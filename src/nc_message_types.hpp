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
    Heartbeat = 0,
    HeartbeatOK = 1,
    HeartbeatError = 2,
    Init = 3,
    InitOK = 4,
    InitError = 5,
    NewDataFromServer = 6,
    NewResultFromNode = 7,
    NodeNeedsMoreData = 8,
    ResultOK = 9,
    ConnectionError = 10,
    Quit = 11
};

uint8_t const NC_NONCE_LENGTH = 12;
uint8_t const NC_GCM_TAG_LENGTH = 16;

struct NCRawMessage {
    std::vector<uint8_t> data = {};
};

struct NCCompressedMessage {
    std::vector<uint8_t> data = {};
};

struct NCEncodedMessage {
    std::vector<uint8_t> nonce = std::vector<uint8_t>(NC_NONCE_LENGTH);
    std::vector<uint8_t> tag = std::vector<uint8_t>(NC_GCM_TAG_LENGTH);
    std::vector<uint8_t> data = {};
};

struct NCDecodedMessage {
    std::vector<uint8_t> data = {};
};

struct NCNodeMessage {
    std::vector<uint8_t> data = {};
};

struct NCServerMessage {
    std::vector<uint8_t> data = {};
};

#endif // FILE_NC_MESSAGE_TYPES_HPP_INCLUDED