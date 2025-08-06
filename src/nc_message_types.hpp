/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines various message types
*/

#ifndef FILE_NC_MESSAGE_TYPES_HPP_INCLUDED
#define FILE_NC_MESSAGE_TYPES_HPP_INCLUDED

// STD includes:
#include <cstdint>
#include <vector>

// Local includes:
#include "nc_nodeid.hpp"

enum struct NCMessageType: uint8_t {
    Unknown = 0,
    Heartbeat,
    HeartbeatOK,
    HeartbeatError,
    Init,
    InitOK,
    InitError,
    NewDataFromServer,
    NewResultFromNode,
    NodeNeedsMoreData,
    ResultOK,
    ConnectionError,
    Quit
};

/*
template <>
struct fmt::formatter<NCMessageType> {
    constexpr auto parse(fmt::format_parse_context& ctx) {
        return ctx.begin();
    }

    template <typename Context>
    auto format(const NCMessageType& msg_error, Context& ctx) {
        std::string_view name = "Unknown message";
        switch (msg_error) {
            case NCMessageType::Unknown:
                name = "Unknown";
                break;
            case NCMessageType::Heartbeat:
                name = "Heartbeat";
                break;
            case NCMessageType::HeartbeatOK:
                name = "HeartbeatOK";
                break;
            case NCMessageType::HeartbeatError:
                name = "HeartbeatError";
                break;
            case NCMessageType::Init:
                name = "Init";
                break;
            case NCMessageType::InitOK:
                name = "InitOK";
                break;
            case NCMessageType::InitError:
                name = "InitError";
                break;
            case NCMessageType::NewDataFromServer:
                name = "NewDataFromServer";
                break;
            case NCMessageType::NewResultFromNode:
                name = "NewResultFromNode";
                break;
            case NCMessageType::NodeNeedsMoreData:
                name = "NodeNeedsMoreData";
                break;
            case NCMessageType::ResultOK:
                name = "ResultOK";
                break;
            case NCMessageType::ConnectionError:
                name = "ConnectionError";
                break;
            case NCMessageType::Quit:
                name = "Quit";
                break;
        }

        return fmt::format_to(ctx.out(), "{}", name);
    }
};
*/

uint8_t const NC_NONCE_LENGTH = 12;
uint8_t const NC_GCM_TAG_LENGTH = 16;

struct NCCompressedMessage {
    std::vector<uint8_t> data = {};
};

struct NCDecompressedMessage {
    std::vector<uint8_t> data = {};
};

struct NCEncryptedMessage {
    std::vector<uint8_t> nonce = std::vector<uint8_t>(NC_NONCE_LENGTH);
    std::vector<uint8_t> tag = std::vector<uint8_t>(NC_GCM_TAG_LENGTH);
    std::vector<uint8_t> data = {};
};

struct NCDecryptedMessage {
    std::vector<uint8_t> data = {};
};

struct NCEncodedMessageToServer {
    std::vector<uint8_t> data = {};
};

struct NCEncodedMessageToNode {
    std::vector<uint8_t> data = {};
};

struct NCDecodedMessageFromNode {
    NCMessageType msg_type = NCMessageType::Unknown;
    NCNodeID node_id = NCNodeID();
    std::vector<uint8_t> data = {};
};

struct NCDecodedMessageFromServer {
    NCMessageType msg_type = NCMessageType::Unknown;
    std::vector<uint8_t> data = {};
};

#endif // FILE_NC_MESSAGE_TYPES_HPP_INCLUDED