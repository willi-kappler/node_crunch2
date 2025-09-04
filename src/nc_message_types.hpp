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

namespace NodeCrunch2 {
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
}

#endif // FILE_NC_MESSAGE_TYPES_HPP_INCLUDED