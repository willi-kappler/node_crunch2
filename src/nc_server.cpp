/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines the server class
*/

// STD includes:
#include <thread>
#include <chrono>

// External includes:
#include <spdlog/spdlog.h>
#include <asio.hpp>

// Local includes:
#include "nc_network.hpp"
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

}

void NCServer::nc_register_new_node(NCNodeID node_id) {
    std::chrono::steady_clock clock;
    std::chrono::time_point node_time = clock.now();
    const std::lock_guard<std::mutex> lock(server_mutex);

    if (all_nodes.contains(node_id)) {
        spdlog::debug("Node already registered: {}", node_id.id);
    }

    all_nodes[node_id] = node_time;
}

void NCServer::nc_update_node_time(NCNodeID node_id) {
    std::chrono::steady_clock clock;
    std::chrono::time_point node_time = clock.now();
    const std::lock_guard<std::mutex> lock(server_mutex);

    all_nodes[node_id] = node_time;
}

void NCServer::nc_handle_node() {
    //uint8_t quit_counter;

}

void NCServer::check_heartbeat() {
    std::chrono::steady_clock clock;
    std::chrono::time_point current_time = clock.now();
    auto const sleep_time = std::chrono::seconds(heartbeat_timeout);

    while (!quit.load()) {
        std::this_thread::sleep_for(sleep_time);
        current_time = clock.now();

        const std::lock_guard<std::mutex> lock(server_mutex);
        for (const auto& [node_id, node_time] : all_nodes) {
            auto const time_diff2 = std::chrono::duration_cast<std::chrono::seconds>(current_time - node_time);
            if (time_diff2 > sleep_time) {
                nc_node_timeout(node_id);
            }
        }
    }
}
