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
    io_context(),
    resolver(io_context),
    endpoints(),
    socket(io_context)
    {
}

void NCNode::nc_run() {
    endpoints = resolver.resolve(server_address, std::to_string(server_port));

    while (!quit.load()) {
    }
}

NCExpDecFromServer NCNode::nc_send_msg_return_answer(NCExpEncToServer const& message) {
    return message.and_then([this](NCEncodedMessageToServer message2) mutable -> NCExpDecFromServer {
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

    /*
    NCDecodedMessageFromServer result;

    if (!message) {
        return std::unexpected(message.error());
    }

    return result;
    */
}

void NCNode::nc_send_heartbeat() {
    auto const sleep_time = std::chrono::seconds(heartbeat_timeout);
    auto const heartbeat_message = nc_gen_heartbeat_message(node_id, secret_key);
    uint32_t error_counter = 0;
    NCExpDecFromServer result;

    while (!quit.load()) {
        std::this_thread::sleep_for(sleep_time);
        result = nc_send_msg_return_answer(heartbeat_message);

        if (!result.has_value()) {
            // Log error and continue.
            // Increase error_counter.
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
    }

    // Log that job is done.
}
