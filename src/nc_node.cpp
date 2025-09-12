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

NCNode::NCNode(NCConfiguration config):
    config_intern(config),
    node_id(NCNodeID()),
    quit(false),
    max_error_count(5),
    message_codec_intern(NCMessageCodecNode(node_id, config.secret_key))
    {}

void NCNode::nc_run() {
    spdlog::info("NCNode::nc_run() - starting node");
    NCEncodedMessageToServer const init_message = message_codec_intern.nc_gen_init_message();
    NCEncodedMessageToServer const need_more_data_message = message_codec_intern.nc_gen_need_more_data_message();
    // TODO: make this configurable:
    auto const sleep_time = std::chrono::seconds(60);

    asio::io_context io_context;
    tcp::resolver resolver(io_context);
    tcp::socket socket(io_context);
    tcp::resolver::results_type endpoints = resolver.resolve(config_intern.server_address, std::to_string(config_intern.server_port));

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
                    result = nc_send_msg_return_answer(init_message, socket, endpoints);
                break;
                case NCRunState::NeedData:
                    spdlog::debug("Need data state, send need more data message");
                    result = nc_send_msg_return_answer(need_more_data_message, socket, endpoints);
                break;
                case NCRunState::HasData:
                    spdlog::debug("Has data state, send result message");
                    result_message = message_codec_intern.nc_gen_result_message(new_data);
                    result = nc_send_msg_return_answer(result_message, socket, endpoints);
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
                nc_init(result.data);
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
                new_data = nc_process_data(result.data);
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

[[nodiscard]] NCDecodedMessageFromServer NCNode::nc_send_msg_return_answer(NCEncodedMessageToServer const& message,
        tcp::socket &socket, tcp::resolver::results_type &endpoints) const {
    asio::connect(socket, endpoints);
    nc_send_data(message.data, socket);
    NCEncodedMessageToNode message2{nc_receive_data(socket)};
    return message_codec_intern.nc_decode_message_from_server(message2);
}

void NCNode::nc_send_heartbeat() {
    spdlog::info("NCNode::nc_send_heartbeat() - starting heartbeat thread.");
    auto const sleep_time = std::chrono::seconds(config_intern.heartbeat_timeout);
    auto const heartbeat_message = message_codec_intern.nc_gen_heartbeat_message();
    uint8_t error_counter = 0;
    NCDecodedMessageFromServer result;

    asio::io_context io_context;
    asio::ip::tcp::resolver resolver(io_context);
    asio::ip::tcp::socket socket(io_context);
    asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(
        config_intern.server_address, std::to_string(config_intern.server_port));

    while (!quit.load()) {
        std::this_thread::sleep_for(sleep_time);
        try {
            result = nc_send_msg_return_answer(heartbeat_message, socket, endpoints);
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
