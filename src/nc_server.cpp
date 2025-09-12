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
#include <spdlog/spdlog.h>

// Local includes:
#include "nc_network.hpp"
#include "nc_message.hpp"
#include "nc_server.hpp"
#include "nc_util.hpp"

namespace NodeCrunch2 {
NCServer::NCServer(NCConfiguration config):
    config_intern(config),
    quit(false),
    all_nodes(),
    server_mutex(),
    server_codec_intern(config.secret_key)
{}

void NCServer::nc_run() {
    spdlog::info("NCServer::nc_run() - starting server");

    // Make threahsold configurable
    const uint32_t max_thread_count = 10;

    NCNetworkServer network_server(config_intern.server_port);
    NCNetworkSocket socket;

    // Have to use lambda in order to call non-static method:
    std::thread heartbeat_thread([this] () {nc_check_heartbeat();});

    // Keep track of client threads
    std::queue<std::thread> client_threads;

    while (!quit.load()) {
        // Wait for a client to connect
        try {
            socket = network_server.nc_accept();
        } catch (asio::system_error &e) {
            spdlog::error("Could not accept connection from socket: {}", e.what());
            quit.store(true);
            continue;
        }

        client_threads.emplace([this] (auto sock2) {nc_handle_node(sock2);}, std::move(socket));

        if (client_threads.size() > max_thread_count) {
            client_threads.front().join();
            client_threads.pop();
        }
    }

    spdlog::debug("Waiting for heartbeat thread...");
    heartbeat_thread.join();

    spdlog::debug("Waiting for client threads...");
    while (client_threads.size() > 0) {
        client_threads.front().join();
        client_threads.pop();
    }

    spdlog::info("Will exit now.");
}

bool NCServer::nc_valid_node_id(NCNodeID node_id) {
    const std::lock_guard<std::mutex> lock(server_mutex);
    if (all_nodes.contains(node_id)) {
        return true;
    } else {
        spdlog::error("Unknown node id: {}", node_id.id);
        return false;
    }
}

void NCServer::nc_register_new_node(NCNodeID node_id) {
    spdlog::info("NCServer::nc_register_new_node(), node_id: {}", node_id.id);

    std::chrono::steady_clock clock;
    std::chrono::time_point node_time = clock.now();

    const std::lock_guard<std::mutex> lock(server_mutex);
    if (all_nodes.contains(node_id)) {
        spdlog::debug("Node already registered: {}", node_id.id);
    }

    all_nodes[node_id] = node_time;
}

void NCServer::nc_update_node_time(NCNodeID node_id) {
    spdlog::debug("NCServer::nc_update_node_time(), node_id: {}", node_id.id);

    std::chrono::steady_clock clock;
    std::chrono::time_point node_time = clock.now();

    const std::lock_guard<std::mutex> lock(server_mutex);
    all_nodes[node_id] = node_time;
}

void NCServer::nc_handle_node(NCNetworkSocket& socket) {
    //spdlog::debug("NCServer::nc_handle_node(), ip: {}", sock.remote_endpoint().address().to_string());
    spdlog::debug("NCServer::nc_handle_node(), ip: {}", socket.nc_address());
    //uint8_t quit_counter;
    std::vector<uint8_t> message = socket.nc_receive_data();
    NCDecodedMessageFromNode node_message = server_codec_intern.nc_decode_message_from_node(NCEncodedMessageToServer(message));
    NCNodeID const node_id = node_message.node_id;

    switch (node_message.msg_type) {
        case NCNodeMessageType::Init:
            nc_register_new_node(node_id);
        break;
        case NCNodeMessageType::Heartbeat:
            if (nc_valid_node_id(node_id)) {
                nc_update_node_time(node_id);
            }
        break;
        case NCNodeMessageType::NodeNeedsMoreData:
            if (nc_valid_node_id(node_id)) {
                NCEncodedMessageToNode msg_to_node = server_codec_intern.nc_gen_new_data_message(nc_get_new_data(node_id));
                socket.nc_send_data(msg_to_node.data);
            }
        break;
        case NCNodeMessageType::NewResultFromNode:
            if (nc_valid_node_id(node_id)) {
                nc_process_result(node_id, node_message.data);
                NCEncodedMessageToNode msg_to_node = server_codec_intern.nc_gen_result_ok_message();
                socket.nc_send_data(msg_to_node.data);
            }
        break;
        default:
            spdlog::error("Unexpected message from node: {}", nc_type_to_string(node_message.msg_type));
    }
}

void NCServer::nc_check_heartbeat() {
    std::chrono::steady_clock clock;
    std::chrono::time_point current_time = clock.now();
    auto const sleep_time = std::chrono::seconds(config_intern.heartbeat_timeout);

    while (!quit.load()) {
        std::this_thread::sleep_for(sleep_time);
        current_time = clock.now();

        const std::lock_guard<std::mutex> lock(server_mutex);
        for (const auto& [node_id, node_time]: all_nodes) {
            auto const time_diff = std::chrono::duration_cast<std::chrono::seconds>(current_time - node_time);
            if (time_diff > sleep_time) {
                spdlog::debug("Node timeout: {}", node_id.id);
                nc_node_timeout(node_id);
            }
        }
    }
}
}
