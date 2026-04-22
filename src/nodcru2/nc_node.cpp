/*
    Node Crunch2
    SPDX-License-Identifier: MIT
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines the node class
*/

// STD includes:
#include <thread>
#include <chrono>
#include <tuple>

// External includes:
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/fmt/bundled/ranges.h>

// Local includes:
#include "nc_node.hpp"
#include "nc_network.hpp"
#include "nc_util.hpp"
#include "nc_exceptions.hpp"

namespace nodcru2 {
enum struct NCRunState: uint8_t {
    Init,
    NeedData,
    HasData
};

void NCNodeDataProcessor::nc_init([[maybe_unused]] std::vector<uint8_t> data,
    [[maybe_unused]] NCNodeID node_id) {
}

[[nodiscard]] std::vector<uint8_t> NCNodeDataProcessor::nc_process_data([[maybe_unused]] std::vector<uint8_t> data) {
    return std::vector<uint8_t>();
}

NCNode::NCNode(NCConfiguration config,
    std::shared_ptr<NCNodeDataProcessor> data_processor,
    std::unique_ptr<NCMessageCodecNode> message_codec,
    std::unique_ptr<NCNetworkClientBase> network_client):
    config_intern(config),
    nc_logger(),
    node_id(NCNodeID()),
    quit(false),
    max_error_count(5), // TODO: make this configurable
    node_mutex(),
    message_codec_intern(std::move(message_codec)),
    network_client_intern(std::move(network_client)),
    data_processor_intern(data_processor)
    {
        spdlog::drop("nc_logger");

        if (config_intern.nc_node_log_file.size() > 0) {
            std::string file_name = nodcru2::nc_gen_log_file_name(config_intern.nc_node_log_file);
            nc_logger = spdlog::basic_logger_mt("nc_logger", file_name);
        } else {
            nc_logger = spdlog::stdout_logger_mt("nc_logger");
        }

        if (config_intern.nc_node_log_level == "debug") {
            nc_logger->set_level(spdlog::level::level_enum::debug);
        } else if ((config_intern.nc_node_log_level == "info") || (config_intern.nc_node_log_level.size() == 0)) {
            nc_logger->set_level(spdlog::level::level_enum::info);
        } else if (config_intern.nc_node_log_level == "warn") {
            nc_logger->set_level(spdlog::level::level_enum::warn);
        } else if (config_intern.nc_node_log_level == "error") {
            nc_logger->set_level(spdlog::level::level_enum::err);
        } else {
            throw NCConfigurationException(fmt::format("Unknown log level: {}", config_intern.nc_node_log_level).c_str());
        }
    }

NCNode::NCNode(NCConfiguration config,
    std::shared_ptr<NCNodeDataProcessor> data_processor,
    std::unique_ptr<NCMessageCodecNode> message_codec):
    NCNode(config,
        data_processor,
        std::move(message_codec),
        std::make_unique<NCNetworkClient>(config.server_address, config.server_port))
    {}

NCNode::NCNode(NCConfiguration config,
    std::shared_ptr<NCNodeDataProcessor> data_processor,
    std::unique_ptr<NCNetworkClientBase> network_client):
    NCNode(config,
        data_processor,
        std::make_unique<NCMessageCodecNode>(config.secret_key),
        std::move(network_client))
    {}

NCNode::NCNode(NCConfiguration config,
    std::shared_ptr<NCNodeDataProcessor> data_processor):
    NCNode(config,
        data_processor,
        std::make_unique<NCMessageCodecNode>(config.secret_key),
        std::make_unique<NCNetworkClient>(config.server_address, config.server_port))
    {}

void NCNode::nc_run() {
    nc_logger->info("NCNode::nc_run() - starting node");
    NCEncodedMessageToServer const init_message = message_codec_intern->nc_gen_init_message(node_id);
    NCEncodedMessageToServer const need_more_data_message = message_codec_intern->nc_gen_need_more_data_message(node_id);
    // TODO: make this configurable:
    auto const sleep_time = std::chrono::seconds(10);

    uint8_t error_counter = 0;
    NCDecodedMessageFromServer result;
    NCEncodedMessageToServer result_message;
    NCRunState run_state = NCRunState::Init;
    std::vector<uint8_t> new_data;

    // Have to use lambda in order to call non-static method:
    std::thread heartbeat_thread([this](){nc_send_heartbeat();});

    while (!quit.load()) {
        if (error_counter >= max_error_count) {
            // Too many errors, quit now.
            nc_logger->error("Too many errors: {}, will exit now.", error_counter);
            quit.store(true);
            break;
        }

        try {
            switch (run_state) {
                case NCRunState::Init:
                    nc_logger->debug("Init state, send init message");
                    result = nc_send_msg_return_answer(init_message);
                break;
                case NCRunState::NeedData:
                    nc_logger->debug("Need data state, send need more data message");
                    result = nc_send_msg_return_answer(need_more_data_message);
                break;
                case NCRunState::HasData:
                    nc_logger->debug("Has data state, send result message");
                    result_message = message_codec_intern->nc_gen_result_message(new_data, node_id);
                    result = nc_send_msg_return_answer(result_message);
                break;
                default:
                    // Unknown state, should not happen, quit now.
                    nc_logger->error("Unknown state: {}", static_cast<uint8_t>(run_state));
                    quit.store(true);
                    continue;
            }
        } catch (std::exception &e) {
            error_counter++;
            nc_logger->error("Caught exception: {}", e.what());
            std::this_thread::sleep_for(sleep_time);
            continue;
        }

        switch (result.msg_type) {
            case NCServerMessageType::InitOK:
                nc_logger->debug("InitOK from server.");
                data_processor_intern->nc_init(result.data, node_id);
                run_state = NCRunState::NeedData;
            break;
            case NCServerMessageType::InvalidNodeID:
                // Invalid node id was sent to the server.
                // This node hasn't registered yet to the server.
                error_counter++;
                nc_logger->error("InvalidNodeID from server, error counter: {}", error_counter);
                std::this_thread::sleep_for(sleep_time);
            break;
            case NCServerMessageType::NewDataFromServer:
                // Received new data from server.
                nc_logger->debug("New data from server.");
                new_data = data_processor_intern->nc_process_data(result.data);
                run_state = NCRunState::HasData;
            break;
            case NCServerMessageType::ResultOK:
                // Result was accepted by server.
                // Request more data.
                nc_logger->debug("ResultOK from server.");
                run_state = NCRunState::NeedData;
            break;
            case NCServerMessageType::Quit:
                // Job is done.
                nc_logger->info("Quit from server, will exit now.");
                quit.store(true);
            break;
            default:
                // Unknown message.
                error_counter++;
                nc_logger->error("Unknown message: {}, error counter: {}", nc_type_to_string(result.msg_type), error_counter);
                std::this_thread::sleep_for(sleep_time);
        }
    }

    nc_logger->debug("Waiting for heartbeat thread...");
    heartbeat_thread.join();
    nc_logger->info("Will exit now.");
    nc_logger->flush();

    // Wait for all log files to be written
    // TODO: make this duration configurable:
    std::this_thread::sleep_for(sleep_time);
}

[[nodiscard]] NCNodeID NCNode::nc_get_node_id() {
    return node_id;
}

[[nodiscard]] NCDecodedMessageFromServer NCNode::nc_send_msg_return_answer(NCEncodedMessageToServer const& message) {
    const std::lock_guard<std::mutex> lock(node_mutex);
    std::unique_ptr<NCNetworkSocketBase> socket = network_client_intern->nc_connect();
    socket->nc_send_data(message.data);
    NCEncodedMessageToNode message2;
    message2.data = socket->nc_receive_data();
    return message_codec_intern->nc_decode_message_from_server(message2);
}

void NCNode::nc_send_heartbeat() {
    nc_logger->info("NCNode::nc_send_heartbeat() - starting heartbeat thread.");
    auto const sleep_time = std::chrono::seconds(config_intern.heartbeat_timeout);
    auto const heartbeat_message = message_codec_intern->nc_gen_heartbeat_message(node_id);
    uint8_t error_counter = 0;
    NCDecodedMessageFromServer result;

    while (!quit.load()) {
        std::this_thread::sleep_for(sleep_time);

        if (quit.load()) {
            // No need for sending any heartbeats...
            break;
        }

        try {
            result = nc_send_msg_return_answer(heartbeat_message);
        } catch (std::exception &e) {
            error_counter++;
            nc_logger->error("HB, Caught exception: {}", e.what());
            continue;
        }

        switch (result.msg_type) {
            case NCServerMessageType::HeartbeatOK:
                nc_logger->debug("HB, HeartbeatOK from server.");
                // Everything OK, nothing to do.
            break;
            case NCServerMessageType::InvalidNodeID:
                // Invalid node id was sent to the server.
                // This node hasn't registered yet to the server.
                error_counter++;
                nc_logger->error("HB, InvalidNodeID from server, error counter: {}", error_counter);
            break;
            case NCServerMessageType::Quit:
                // Job is done, so we can quit.
                nc_logger->info("HB, Quit from server, will exit now.");
                quit.store(true);
            break;
            default:
                // Increase error_counter.
                error_counter++;
                nc_logger->error("HB, Unknown message: {}, error counter: {}", nc_type_to_string(result.msg_type), error_counter);
        }

        if (error_counter >= max_error_count) {
            // Too many errors, quit now.
            nc_logger->error("HB, Too many errors: {}, will exit now.", error_counter);
            quit.store(true);
        }
    }

    nc_logger->info("HB, Will exit now.");
}

void NCNode::nc_set_logger(std::shared_ptr<spdlog::logger> logger) {
    spdlog::drop("nc_logger");
    nc_logger = logger;
}
}
