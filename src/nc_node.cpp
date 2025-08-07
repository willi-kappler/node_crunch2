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

enum struct NCRunState: uint8_t {
    Init,
    NeedData,
    HasData
};

NCNode::NCNode(NCConfiguration config):
    server_address(config.server_address),
    server_port(config.server_port),
    heartbeat_timeout(config.heartbeat_timeout),
    secret_key(config.secret_key),
    node_id(NCNodeID()),
    quit(false),
    max_error_count(5)
    {}

void NCNode::nc_run() {
    spdlog::info("NCNode::nc_run() - starting node");
    NCExpEncToServer const init_message = nc_gen_init_message(node_id, secret_key);
    NCExpEncToServer const need_more_data_message = nc_gen_need_more_data_message(node_id, secret_key);
    // TODO: make this configurable:
    auto const sleep_time = std::chrono::seconds(60);

    asio::io_context io_context;
    tcp::resolver resolver(io_context);
    tcp::socket socket(io_context);
    tcp::resolver::results_type endpoints = resolver.resolve(server_address, std::to_string(server_port));

    uint8_t error_counter = 0;
    NCExpDecFromServer result;
    NCExpEncToServer result_message;
    NCRunState run_state = NCRunState::Init;
    std::vector<uint8_t> new_data;

    // Have to use lambda in order to call non-static method:
    std::thread heartbeat_thread([this](){nc_send_heartbeat();});

    while (!quit.load()) {
        switch (run_state) {
            case NCRunState::Init:
                spdlog::debug("Init state, send init message");
                result = nc_send_msg_return_answer(init_message, socket, endpoints);
            break;
            case NCRunState::NeedData:
                spdlog::debug("Need data state, send need more data message");
                result = nc_send_msg_return_answer(need_more_data_message, socket, endpoints);
            break;
            case NCRunState::HasData:
                spdlog::debug("Has data state, send result message");
                result_message = nc_gen_result_message(node_id, new_data, secret_key);
                result = nc_send_msg_return_answer(result_message, socket, endpoints);
            break;
            default:
                // Unknown state, should not happen, quit now.
                spdlog::error("Unknown state: {}", static_cast<uint8_t>(run_state));
                quit.store(true);
                continue;
        }

        if (!result.has_value()) {
            error_counter++;
            spdlog::error("Result error: {}, error counter: {}", nc_error_to_str(result.error()), error_counter);
            std::this_thread::sleep_for(sleep_time);
        } else {
            switch (result->msg_type) {
                case NCMessageType::InitOK:
                    spdlog::debug("InitOK from server.");
                    nc_init(result->data);
                    run_state = NCRunState::NeedData;
                break;
                case NCMessageType::InitError:
                    // Error at initialisation.
                    error_counter++;
                    spdlog::error("InitError from server, error counter: {}", error_counter);
                    std::this_thread::sleep_for(sleep_time);
                break;
                case NCMessageType::NewDataFromServer:
                    // Received new data from server.
                    spdlog::debug("New data from server.");
                    new_data = nc_process_data(result->data);
                    run_state = NCRunState::HasData;
                break;
                case NCMessageType::ResultOK:
                    // Result was accepted by server.
                    // Request more data.
                    spdlog::debug("ResultOK from server.");
                    run_state = NCRunState::NeedData;
                break;
                case NCMessageType::Quit:
                    // Job is done.
                    spdlog::info("Quit from server, will exit now.");
                    quit.store(true);
                break;
                default:
                    // Unknown message
                    error_counter++;
                    spdlog::error("Unknown message: {}, error counter: {}", nc_type_to_string(result->msg_type), error_counter);
                    std::this_thread::sleep_for(sleep_time);
            }
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

[[nodiscard]] NCExpDecFromServer NCNode::nc_send_msg_return_answer(NCExpEncToServer const& message,
        tcp::socket &socket, tcp::resolver::results_type &endpoints) {
    return message.and_then([this, &socket, &endpoints](NCEncodedMessageToServer message2) mutable -> NCExpDecFromServer {
        // Connect to the server
        try {
            // tcp::endpoint ep = asio::connect(socket, endpoints);
            asio::connect(socket, endpoints);
        } catch (const std::exception& e) {
            spdlog::error("ASIO connect exception: {}", e.what());
            return std::unexpected(NCMessageError::NetworkConnectError);
        }

        return nc_send_data(message2.data, socket).and_then([this, &socket](uint8_t v){
            std::ignore = v;
            return nc_receive_data(socket);
        }).and_then([this](std::vector<uint8_t> raw_data){
            return nc_decode_message_from_server(NCEncodedMessageToNode(raw_data), secret_key);
        });
    });
}

void NCNode::nc_send_heartbeat() {
    spdlog::info("NCNode::nc_send_heartbeat() - starting heartbeat thread.");
    auto const sleep_time = std::chrono::seconds(heartbeat_timeout);
    auto const heartbeat_message = nc_gen_heartbeat_message(node_id, secret_key);
    uint8_t error_counter = 0;
    NCExpDecFromServer result;

    asio::io_context io_context;
    asio::ip::tcp::resolver resolver(io_context);
    asio::ip::tcp::socket socket(io_context);
    asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(server_address, std::to_string(server_port));

    while (!quit.load()) {
        std::this_thread::sleep_for(sleep_time);
        result = nc_send_msg_return_answer(heartbeat_message, socket, endpoints);

        if (!result.has_value()) {
            error_counter++;
            spdlog::error("Result error: {}, error counter: {}", nc_error_to_str(result.error()), error_counter);
        } else {
            switch (result->msg_type) {
                case NCMessageType::HeartbeatOK:
                    spdlog::debug("HeartbeatOK from server.");
                    // Everything OK, nothing to do.
                break;
                case NCMessageType::HeartbeatError:
                    // Heartbeat was not sent in time.
                    error_counter++;
                    spdlog::error("HeartbetaError from server, error counter: {}", error_counter);
                break;
                case NCMessageType::Quit:
                    // Job is done, so we can quit.
                    spdlog::info("Quit from server, will exit now.");
                    quit.store(true);
                break;
                default:
                    // Increase error_counter.
                    error_counter++;
                    spdlog::error("Unknown message: {}, error counter: {}", nc_type_to_string(result->msg_type), error_counter);
            }
        }

        if (error_counter >= max_error_count) {
            // Too many errors, quit now.
            spdlog::error("Too many errors: {}, will exit now.", error_counter);
            quit.store(true);
        }
    }

    spdlog::info("Will exit now.");
}
