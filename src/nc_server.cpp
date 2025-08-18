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
#include <asio.hpp>

// Local includes:
#include "nc_network.hpp"
#include "nc_message.hpp"
#include "nc_server.hpp"

using asio::ip::tcp;

NCServer::NCServer(NCConfiguration config):
    server_address(config.server_address),
    server_port(config.server_port),
    heartbeat_timeout(config.heartbeat_timeout),
    secret_key(config.secret_key),
    quit(false),
    all_nodes(),
    server_mutex()
{}

void NCServer::nc_run() {
    spdlog::info("NCServer::nc_run() - starting server");

    // Make threahsold configurable
    const uint32_t max_thread_count = 10;

    asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), server_port));
    tcp::socket sock(io_context);

    // Have to use lambda in order to call non-static method:
    std::thread heartbeat_thread([this] () {nc_check_heartbeat();});

    // Keep track of client threads
    std::queue<std::thread> client_threads;

    while (!quit.load()) {
        // Wait for a client to connect
        try {
            acceptor.accept(sock);
        } catch (asio::system_error &e) {
            spdlog::error("Could not accept connection from socket: {}", e.what());
            quit.store(true);
            continue;
        }

        client_threads.emplace([this] (auto sock2) {nc_handle_node(sock2);}, std::move(sock));

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

void NCServer::nc_handle_node(tcp::socket& sock) {
    spdlog::debug("NCServer::nc_handle_node(), ip: {}", sock.remote_endpoint().address().to_string());
    //uint8_t quit_counter;

    nc_receive_data(sock).and_then([this, &sock](std::vector<uint8_t> message) {
        return nc_decode_message_from_node(NCEncodedMessageToServer(message), secret_key);
    }).and_then([this, &sock] (NCDecodedMessageFromNode node_message) {
        NCNodeID const node_id = node_message.node_id;

        switch (node_message.msg_type) {
            case NCMessageType::Init:
                nc_register_new_node(node_id);
            break;
            case NCMessageType::Heartbeat:
                if (nc_valid_node_id(node_id)) {
                    nc_update_node_time(node_id);
                }
            break;
            case NCMessageType::NodeNeedsMoreData:
                if (nc_valid_node_id(node_id)) {
                    nc_gen_new_data_message(nc_get_new_data(node_id),
                        secret_key).and_then([&sock] (NCEncodedMessageToNode msg_to_node) {
                        return nc_send_data(msg_to_node.data, sock);
                    }).or_else([] (NCMessageError msg_error) {
                        spdlog::error("Error while sending data to node: {}", nc_error_to_str(msg_error));
                        return std::expected<uint8_t, NCMessageError>(0);
                    });
                }
            break;
            case NCMessageType::NewResultFromNode:
                if (nc_valid_node_id(node_id)) {
                    nc_process_result(node_id, node_message.data);
                    nc_gen_result_ok_message(secret_key).and_then([&sock] (NCEncodedMessageToNode msg_to_node) {
                        return nc_send_data(msg_to_node.data, sock);
                    }).or_else([] (NCMessageError msg_error) {
                        spdlog::error("Error while sending data to node: {}", nc_error_to_str(msg_error));
                        return std::expected<uint8_t, NCMessageError>(0);
                    });
                }
            break;
            default:
                spdlog::error("Unexpected message from node: {}", nc_type_to_string(node_message.msg_type));
        }

        return std::expected<uint8_t, NCMessageError>(0);
    }).or_else([] (NCMessageError msg_error) {
        spdlog::error("Error while handleing node: {}", nc_error_to_str(msg_error));
        return std::expected<uint8_t, NCMessageError>(0);
    });
}

void NCServer::nc_check_heartbeat() {
    std::chrono::steady_clock clock;
    std::chrono::time_point current_time = clock.now();
    auto const sleep_time = std::chrono::seconds(heartbeat_timeout);

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
