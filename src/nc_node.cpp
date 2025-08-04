/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines the node class
*/

// STD includes:
#include <thread>
#include <chrono>

// Local includes:
#include "nc_node.hpp"

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

    // TODO: start background thread for heartbeats.

    while (!quit.load()) {
        switch (run_state) {
            case NCRunState::Init:
                result = nc_send_msg_return_answer(init_message, socket, endpoints);
            break;
            case NCRunState::NeedData:
                result = nc_send_msg_return_answer(need_more_data_message, socket, endpoints);
            break;
            case NCRunState::HasData:
                result_message = nc_gen_result_message(node_id, new_data, secret_key);
                result = nc_send_msg_return_answer(result_message, socket, endpoints);
            break;
            default:
                // Unknown state, should not happen, quit now.
                // TODO: log error
                quit.store(true);
                continue;
        }

        if (!result.has_value()) {
            // TODO: log error message with error value.
            error_counter++;
            std::this_thread::sleep_for(sleep_time);
        } else {
            switch (result->msg_type) {
                case NCMessageType::InitOK:
                    // TODO: log init ok
                    nc_init(result->data);
                    run_state = NCRunState::NeedData;
                break;
                case NCMessageType::InitError:
                    // Error at initialisation.
                    // TODO log error
                    error_counter++;
                    std::this_thread::sleep_for(sleep_time);
                break;
                case NCMessageType::NewDataFromServer:
                    // Received new data from server.
                    // TODO: log message.
                    new_data = nc_process_data(result->data);
                    run_state = NCRunState::HasData;
                break;
                case NCMessageType::ResultOK:
                    // Result was accepted by server.
                    // Request more data.
                    run_state = NCRunState::NeedData;
                break;
                case NCMessageType::Quit:
                    // Job is done.
                    // TODO: log message.
                    quit.store(true);
                break;
                default:
                    // Unknown message, TODO: log error
                    error_counter++;
                    std::this_thread::sleep_for(sleep_time);
            }
        }

        if (error_counter >= max_error_count) {
            // TODO: log error counter.
            // Too many errors, quit now.
            quit.store(true);
        }
    }
}

NCExpDecFromServer NCNode::nc_send_msg_return_answer(NCExpEncToServer const& message,
        tcp::socket &socket, tcp::resolver::results_type &endpoints) {
    return message.and_then([this, &socket, &endpoints](NCEncodedMessageToServer message2) mutable -> NCExpDecFromServer {
        // Connect to the server
        asio::connect(socket, endpoints);

        NCDecodedMessageFromServer result;

        if (result.msg_type == NCMessageType::Init) {
            if (message2.data.size() > 0) {
                return std::unexpected(NCMessageError::SizeMissmatch);
            }
        }

        return result;
    });
}

void NCNode::nc_send_heartbeat() {
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
            // Log error and continue.
            error_counter++;
        } else {
            switch (result->msg_type) {
                case NCMessageType::HeartbeatOK:
                    // Everything OK, nothing to do.
                break;
                case NCMessageType::HeartbeatError:
                    // Heartbeat was not sent in time.
                    // Log error and increase error_counter.
                    error_counter++;
                break;
                case NCMessageType::Quit:
                    // Job is done, so we can quit.
                    quit.store(true);
                break;
                default:
                    // Log error: unknown message type.
                    // Increase error_counter.
                    error_counter++;
            }
        }

        if (error_counter >= max_error_count) {
            // Too many errors, quit now.
            // TODO: log error counter.
            quit.store(true);
        }
    }

    // Log that job is done.
}
