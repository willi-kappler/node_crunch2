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
    quit(false)
    {}

void NCNode::nc_run() {
    auto const init_message = nc_gen_init_message(node_id, secret_key);
    auto const need_more_data_message = nc_gen_need_more_data_message(node_id, secret_key);
    // nc_gen_result_message(node_id, new_data, secret_key);

    asio::io_context io_context;
    tcp::resolver resolver(io_context);
    tcp::socket socket(io_context);
    tcp::resolver::results_type endpoints = resolver.resolve(server_address, std::to_string(server_port));

    bool init_ok = false;
    uint8_t error_counter = 0;
    NCExpDecFromServer result;
    // TODO: make this configurable:
    auto const sleep_time = std::chrono::seconds(60);

    while (true) {
        result = nc_send_msg_return_answer(init_message, socket, endpoints);

        if (!result.has_value()) {
            // TODO: log error message with error value.
            error_counter++;
        } else {
            switch (result->msg_type) {
                case NCMessageType::InitOK:
                    // TODO: log sucess message.
                    nc_init(result->data);
                    init_ok = true;
                break;
                case NCMessageType::InitError:
                    // TODO: log error message.
                    error_counter++;
                break;
                default:
                    // Log error: unknown message type.
                    error_counter++;
            }
        }

        if (init_ok) {
            break; // while loop
        } else {
            if (error_counter >= 5) {
                // TODO: log error counter.
                // Too many errors, quit now.
                return;
            } else {
                // Give it some more tries after a specific time:
                std::this_thread::sleep_for(sleep_time);
            }
        }
    }

    // TODO: start background thread for heartbeats.

    std::vector<uint8_t> new_data;
    NCExpEncToServer result_message;
    // Reset error counter:
    error_counter = 0;

    while (!quit.load()) {
        result = nc_send_msg_return_answer(need_more_data_message, socket, endpoints);

        if (!result.has_value()) {
            // TODO: log error message with error value.
            error_counter++;
            // Wait some time before retrying:
            std::this_thread::sleep_for(sleep_time);
        } else {
            switch (result->msg_type) {
                case NCMessageType::NewDataFromServer:
                    // TODO: log new data from server.
                    new_data = nc_process_data(result->data);
                    result_message = nc_gen_result_message(node_id, new_data, secret_key);
                    result = nc_send_msg_return_answer(result_message, socket, endpoints);
                    // TODO: Check result.
                break;
                case NCMessageType::Quit:
                    // TODO: log quit message.
                    quit.store(true);
                break;
                default:
                    // Log error: unknown message type.
                    error_counter++;
                    // Wait some time before retrying:
                    std::this_thread::sleep_for(sleep_time);
            }
        }

        if (error_counter >=5) {
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

        if (error_counter >=5) {
            // Too many errors, quit now.
            // TODO: log error counter.
            quit.store(true);
        }
    }

    // Log that job is done.
}
