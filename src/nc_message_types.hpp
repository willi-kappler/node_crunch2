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

struct NCRawMessage {
    NCMessageType msg_type = NCMessageType::Unknown;
    std::string node_id = "";
    std::vector<uint8_t> data = {};
};

struct NCCompressedMessage {
    std::vector<uint8_t> data = {};
};

struct NCDecompressedMessage {
    std::vector<uint8_t> data = {};
};

struct NCEncodedMessage {
    std::vector<uint8_t> nonce = std::vector<uint8_t>(NC_NONCE_LENGTH);
    std::vector<uint8_t> tag = std::vector<uint8_t>(NC_GCM_TAG_LENGTH);
    std::vector<uint8_t> data = {};
};

struct NCDecodedMessage {
    NCMessageType msg_type = NCMessageType::Unknown;
    std::string node_id = "";
    std::vector<uint8_t> data = {};
};

struct NCNodeMessage {
    std::vector<uint8_t> data = {};
};

struct NCServerMessage {
    std::vector<uint8_t> data = {};
};

#endif // FILE_NC_MESSAGE_TYPES_HPP_INCLUDED