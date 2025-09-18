/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines the node class
*/

// STD includes:
#include <thread>
#include <chrono>
#include <tuple>

// External includes:
#include <spdlog/spdlog.h>

// Local includes:
#include "nc_node.hpp"
#include "nc_network.hpp"
#include "nc_util.hpp"

namespace NodeCrunch2 {
enum struct NCRunState: uint8_t {
    Init,
    NeedData,
    HasData
};

void NCNodeDataProcessor::nc_init([[maybe_unused]] std::vector<uint8_t> data) {
}

[[nodiscard]] std::vector<uint8_t> NCNodeDataProcessor::nc_process_data([[maybe_unused]] std::vector<uint8_t> data) {
    return std::vector<uint8_t>();
}

NCNode::NCNode(NCConfiguration config, NCNodeDataProcessor data_processor, NCMessageCodecNode const message_codec, NCNetworkClientBase const network_client):
    config_intern(config),
    node_id(NCNodeID()),
    quit(false),
    max_error_count(5),
    node_mutex(),
    message_codec_intern(message_codec),
    network_client_intern(network_client),
    data_processor_intern(data_processor)
    {}

NCNode::NCNode(NCConfiguration config, NCNodeDataProcessor data_processor, NCMessageCodecNode const message_codec):
    NCNode(config, data_processor, message_codec, NCNetworkClient(config.server_address, config.server_port))
    {}

NCNode::NCNode(NCConfiguration config, NCNodeDataProcessor data_processor, NCNetworkClientBase const network_client):
    NCNode(config, data_processor, NCMessageCodecNode(config.secret_key), network_client)
    {}

NCNode::NCNode(NCConfiguration config, NCNodeDataProcessor data_processor):
    NCNode(config, data_processor, NCMessageCodecNode(config.secret_key), NCNetworkClient(config.server_address, config.server_port))
    {}

void NCNode::nc_run() {
    spdlog::info("NCNode::nc_run() - starting node");
    NCEncodedMessageToServer const init_message = message_codec_intern.nc_gen_init_message(node_id);
    NCEncodedMessageToServer const need_more_data_message = message_codec_intern.nc_gen_need_more_data_message(node_id);
    // TODO: make this configurable:
    auto const sleep_time = std::chrono::seconds(60);

    uint8_t error_counter = 0;
    NCDecodedMessageFromServer result;
    NCEncodedMessageToServer result_message;
    NCRunState run_state = NCRunState::Init;
    std::vector<uint8_t> new_data;

    // Have to use lambda in order to call non-static method:
    std::thread heartbeat_thread([this](){nc_send_heartbeat();});

    while (!quit.load()) {
        try {
            switch (run_state) {
                case NCRunState::Init:
                    spdlog::debug("Init state, send init message");
                    result = nc_send_msg_return_answer(init_message);
                break;
                case NCRunState::NeedData:
                    spdlog::debug("Need data state, send need more data message");
                    result = nc_send_msg_return_answer(need_more_data_message);
                break;
                case NCRunState::HasData:
                    spdlog::debug("Has data state, send result message");
                    result_message = message_codec_intern.nc_gen_result_message(new_data, node_id);
                    result = nc_send_msg_return_answer(result_message);
                break;
                default:
                    // Unknown state, should not happen, quit now.
                    spdlog::error("Unknown state: {}", static_cast<uint8_t>(run_state));
                    quit.store(true);
                    continue;
            }
        } catch (std::exception &e) {
            error_counter++;
            spdlog::error("Caught exception: {}", e.what());
        }

        switch (result.msg_type) {
            case NCServerMessageType::InitOK:
                spdlog::debug("InitOK from server.");
                data_processor_intern.nc_init(result.data);
                run_state = NCRunState::NeedData;
            break;
            case NCServerMessageType::InitError:
                // Error at initialisation.
                error_counter++;
                spdlog::error("InitError from server, error counter: {}", error_counter);
                std::this_thread::sleep_for(sleep_time);
            break;
            case NCServerMessageType::NewDataFromServer:
                // Received new data from server.
                spdlog::debug("New data from server.");
                new_data = data_processor_intern.nc_process_data(result.data);
                run_state = NCRunState::HasData;
            break;
            case NCServerMessageType::ResultOK:
                // Result was accepted by server.
                // Request more data.
                spdlog::debug("ResultOK from server.");
                run_state = NCRunState::NeedData;
            break;
            case NCServerMessageType::Quit:
                // Job is done.
                spdlog::info("Quit from server, will exit now.");
                quit.store(true);
            break;
            default:
                // Unknown message
                error_counter++;
                spdlog::error("Unknown message: {}, error counter: {}", nc_type_to_string(result.msg_type), error_counter);
                std::this_thread::sleep_for(sleep_time);
        }

        if (error_counter >= max_error_count) {
            // Too many errors, quit now.
            spdlog::error("Too many errors: {}, will exit now.", error_counter);
            quit.store(true);
        }
    }

    spdlog::debug("Waiting for heartbeat thread...");
    heartbeat_thread.join();
    spdlog::info("Will exit now.");
}

[[nodiscard]] NCDecodedMessageFromServer NCNode::nc_send_msg_return_answer(NCEncodedMessageToServer const& message) {
    const std::lock_guard<std::mutex> lock(node_mutex);
    NCNetworkSocketBase socket = network_client_intern.nc_connect();
    socket.nc_send_data(message.data);
    NCEncodedMessageToNode message2{socket.nc_receive_data()};
    return message_codec_intern.nc_decode_message_from_server(message2);
}

void NCNode::nc_send_heartbeat() {
    spdlog::info("NCNode::nc_send_heartbeat() - starting heartbeat thread.");
    auto const sleep_time = std::chrono::seconds(config_intern.heartbeat_timeout);
    auto const heartbeat_message = message_codec_intern.nc_gen_heartbeat_message(node_id);
    uint8_t error_counter = 0;
    NCDecodedMessageFromServer result;

    while (!quit.load()) {
        std::this_thread::sleep_for(sleep_time);
        try {
            result = nc_send_msg_return_answer(heartbeat_message);
        } catch (std::exception &e) {
            error_counter++;
            spdlog::error("Caught exception: {}", e.what());
        }

        switch (result.msg_type) {
            case NCServerMessageType::HeartbeatOK:
                spdlog::debug("HeartbeatOK from server.");
                // Everything OK, nothing to do.
            break;
            case NCServerMessageType::HeartbeatError:
                // Heartbeat was not sent in time.
                error_counter++;
                spdlog::error("HeartbetaError from server, error counter: {}", error_counter);
            break;
            case NCServerMessageType::Quit:
                // Job is done, so we can quit.
                spdlog::info("Quit from server, will exit now.");
                quit.store(true);
            break;
            default:
                // Increase error_counter.
                error_counter++;
                spdlog::error("Unknown message: {}, error counter: {}", nc_type_to_string(result.msg_type), error_counter);
        }

        if (error_counter >= max_error_count) {
            // Too many errors, quit now.
            spdlog::error("Too many errors: {}, will exit now.", error_counter);
            quit.store(true);
        }
    }

    spdlog::info("Will exit now.");
}
}
