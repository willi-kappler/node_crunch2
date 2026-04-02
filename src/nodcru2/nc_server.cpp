/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines the server class
*/

// STD includes:
#include <thread>
#include <chrono>
#include <queue>

// External includes:
#include <spdlog/stopwatch.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/fmt/bundled/ranges.h>

// Local includes:
#include "nc_network.hpp"
#include "nc_message.hpp"
#include "nc_server.hpp"
#include "nc_util.hpp"
#include "nc_exceptions.hpp"

namespace nodcru2 {
[[nodiscard]] std::vector<uint8_t> NCServerDataProcessor::nc_get_init_data() {
    return std::vector<uint8_t>();
}

[[nodiscard]] bool NCServerDataProcessor::nc_is_job_done() {
    return true;
}

void NCServerDataProcessor::nc_save_data() {
}

void NCServerDataProcessor::nc_node_timeout([[maybe_unused]] NCNodeID node_id) {
}

[[nodiscard]] std::vector<uint8_t> NCServerDataProcessor::nc_get_new_data([[maybe_unused]] NCNodeID node_id) {
    return std::vector<uint8_t>();
}

void NCServerDataProcessor::nc_process_result([[maybe_unused]] NCNodeID node_id, [[maybe_unused]] std::vector<uint8_t> result) {
}

NCServer::NCServer(NCConfiguration config,
    std::shared_ptr<NCServerDataProcessor> data_processor,
    std::unique_ptr<NCMessageCodecServer> message_codec,
    std::unique_ptr<NCNetworkServerBase> network_server):
    config_intern(config),
    nc_logger(),
    quit(false),
    all_nodes(),
    server_mutex(),
    message_codec_intern(std::move(message_codec)),
    network_server_intern(std::move(network_server)),
    data_processor_intern(data_processor)
    {
        spdlog::drop("nc_logger");

        if (config_intern.nc_server_log_file.size() > 0) {
            std::string file_name = nodcru2::nc_gen_log_file_name(config_intern.nc_server_log_file);
            nc_logger = spdlog::basic_logger_mt("nc_logger", file_name);
        } else {
            nc_logger = spdlog::stdout_logger_mt("nc_logger");
        }

        if (config_intern.nc_server_log_level == "debug") {
            nc_logger->set_level(spdlog::level::level_enum::debug);
        } else if ((config_intern.nc_server_log_level == "info") || (config_intern.nc_server_log_level.size() == 0)) {
            nc_logger->set_level(spdlog::level::level_enum::info);
        } else if (config_intern.nc_server_log_level == "warn") {
            nc_logger->set_level(spdlog::level::level_enum::warn);
        } else if (config_intern.nc_server_log_level == "error") {
            nc_logger->set_level(spdlog::level::level_enum::err);
        } else {
            throw NCConfigurationException(fmt::format("Unknown log level: {}", config_intern.nc_server_log_level).c_str());
        }
    }

NCServer::NCServer(NCConfiguration config,
    std::shared_ptr<NCServerDataProcessor> data_processor,
    std::unique_ptr<NCMessageCodecServer> message_codec):
    NCServer(config,
        std::move(data_processor),
        std::move(message_codec),
        std::make_unique<NCNetworkServer>(config.server_port))
    {}

NCServer::NCServer(NCConfiguration config,
    std::shared_ptr<NCServerDataProcessor> data_processor,
    std::unique_ptr<NCNetworkServerBase> network_server):
    NCServer(config,
        data_processor,
        std::make_unique<NCMessageCodecServer>(config.secret_key),
        std::move(network_server))
    {}

NCServer::NCServer(NCConfiguration config,
    std::shared_ptr<NCServerDataProcessor> data_processor):
    NCServer(config,
        data_processor,
        std::make_unique<NCMessageCodecServer>(config.secret_key),
        std::make_unique<NCNetworkServer>(config.server_port))
    {}

void NCServer::nc_run() {
    nc_logger->info("NCServer::nc_run() - starting server");
    spdlog::stopwatch sw;

    // TODO: Make threahsold configurable
    const uint32_t max_thread_count = 10;

    std::unique_ptr<NCNetworkSocketBase> socket;

    // Have to use lambda in order to call non-static method:
    std::thread heartbeat_thread([this] () {nc_check_heartbeat();});

    // Keep track of client threads
    std::queue<std::thread> client_threads;
    size_t num_of_threads = 0;

    while (!quit.load()) {
        // Wait for a client to connect
        try {
            socket = network_server_intern->nc_accept();
        } catch (asio::system_error &e) {
            nc_logger->error("Could not accept connection from socket: {}", e.what());
            quit.store(true);
            continue;
        }

        client_threads.emplace([this] (auto sock2) {nc_handle_node(sock2);}, std::move(socket));

        if (client_threads.size() > max_thread_count) {
            client_threads.front().join();
            client_threads.pop();
        }
    }

    // Save all data:
    nc_logger->debug("Saving data...");
    data_processor_intern->nc_save_data();

    nc_logger->debug("Waiting for heartbeat thread...");
    heartbeat_thread.join();

    num_of_threads = client_threads.size();
    while (num_of_threads > 0) {
        nc_logger->debug("Waiting for client threads ({})...", num_of_threads);
        client_threads.front().join();
        client_threads.pop();
        num_of_threads = client_threads.size();
    }

    nc_logger->info("Elapsed time: {} sec.", sw);
    nc_logger->info("Will exit now.");

    // Wait for all log files to be written
    // TODO: make this duration configurable:
    auto const sleep_time = std::chrono::seconds(10);
    std::this_thread::sleep_for(sleep_time);
}

bool NCServer::nc_valid_node_id(NCNodeID node_id) {
    const std::lock_guard<std::mutex> lock(server_mutex);
    if (all_nodes.contains(node_id)) {
        return true;
    } else {
        nc_logger->error("Unknown node id: {}", node_id.id);
        return false;
    }
}

void NCServer::nc_register_new_node(NCNodeID node_id) {
    nc_logger->info("NCServer::nc_register_new_node(), node_id: {}", node_id.id);

    std::chrono::steady_clock clock;
    std::chrono::time_point node_time = clock.now();

    const std::lock_guard<std::mutex> lock(server_mutex);
    if (all_nodes.contains(node_id)) {
        nc_logger->debug("Node already registered: {}", node_id.id);
    } else {
        all_nodes[node_id] = node_time;
    }
}

void NCServer::nc_update_node_time(NCNodeID node_id) {
    nc_logger->debug("NCServer::nc_update_node_time(), node_id: {}", node_id.id);

    std::chrono::steady_clock clock;
    std::chrono::time_point node_time = clock.now();

    const std::lock_guard<std::mutex> lock(server_mutex);
    all_nodes[node_id] = node_time;
}

void NCServer::nc_handle_node(std::unique_ptr<NCNetworkSocketBase> &socket) {
    nc_logger->debug("NCServer::nc_handle_node(), ip: {}", socket->nc_address());
    std::vector<uint8_t> message = socket->nc_receive_data();
    NCDecodedMessageFromNode node_message = message_codec_intern->nc_decode_message_from_node(NCEncodedMessageToServer(message));
    NCNodeID const node_id = node_message.node_id;
    NCEncodedMessageToNode msg_to_node;

    if (quit.load()) {
        msg_to_node = message_codec_intern->nc_gen_quit_message();
    }
    else if (data_processor_intern->nc_is_job_done()) {
        quit.store(true);
        msg_to_node = message_codec_intern->nc_gen_quit_message();
    } else {
        switch (node_message.msg_type) {
            case NCNodeMessageType::Init:
                nc_register_new_node(node_id);
                msg_to_node = message_codec_intern->nc_gen_init_message_ok(data_processor_intern->nc_get_init_data());
            break;
            case NCNodeMessageType::Heartbeat:
                if (nc_valid_node_id(node_id)) {
                    nc_logger->debug("Heartbeat from node: {}", node_id.id);
                    nc_update_node_time(node_id);
                    msg_to_node = message_codec_intern->nc_gen_heartbeat_message_ok();
                } else {
                    msg_to_node = message_codec_intern->nc_gen_invalid_node_id_error();
                }
            break;
            case NCNodeMessageType::NodeNeedsMoreData:
                if (nc_valid_node_id(node_id)) {
                    msg_to_node = message_codec_intern->nc_gen_new_data_message(data_processor_intern->nc_get_new_data(node_id));
                } else {
                    msg_to_node = message_codec_intern->nc_gen_invalid_node_id_error();
                }
            break;
            case NCNodeMessageType::NewResultFromNode:
                if (nc_valid_node_id(node_id)) {
                    data_processor_intern->nc_process_result(node_id, node_message.data);
                    msg_to_node = message_codec_intern->nc_gen_result_ok_message();
                } else {
                    msg_to_node = message_codec_intern->nc_gen_invalid_node_id_error();
                }
            break;
            default:
                nc_logger->error("Unexpected message from node: {}", nc_type_to_string(node_message.msg_type));
                msg_to_node = message_codec_intern->nc_gen_unknown_error();
        }
    }

    // Send answer back to node:
    socket->nc_send_data(msg_to_node.data);
}

void NCServer::nc_check_heartbeat() {
    std::chrono::steady_clock clock;
    std::chrono::time_point current_time = clock.now();
    auto const sleep_time = std::chrono::seconds(config_intern.heartbeat_timeout);

    while (!quit.load()) {
        std::this_thread::sleep_for(sleep_time);

        if (quit.load()) {
            // No need for heartbeat checks...
            break;
        }

        current_time = clock.now();

        const std::lock_guard<std::mutex> lock(server_mutex);
        for (const auto& [node_id, node_time]: all_nodes) {
            auto const time_diff = std::chrono::duration_cast<std::chrono::seconds>(current_time - node_time);
            if (time_diff > sleep_time) {
                nc_logger->debug("Node timeout: {}", node_id.id);
                data_processor_intern->nc_node_timeout(node_id);
            }
        }
    }
}

void NCServer::nc_set_logger(std::shared_ptr<spdlog::logger> logger) {
    spdlog::drop("nc_logger");
    nc_logger = logger;
}
}
