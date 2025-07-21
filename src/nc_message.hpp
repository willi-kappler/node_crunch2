/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines the message type and decoding / encoding
*/

#ifndef FILE_NC_MESSAGE_HPP_INCLUDED
#define FILE_NC_MESSAGE_HPP_INCLUDED

// STD includes:
#include <cstdint>
#include <vector>
#include <string>

enum struct NCMessageType : uint8_t {
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

std::vector<uint8_t> encode_message(std::vector<uint8_t> message, std::string secret_key);

std::vector<uint8_t> decode_message(std::vector<uint8_t> message, std::string secret_key);

#endif // FILE_NC_MESSAGE_HPP_INCLUDED