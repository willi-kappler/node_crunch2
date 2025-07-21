/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines the message type and decoding / encoding
*/

// STD includes:
#include <cstdint>
#include <vector>
#include <string>

// Local includes:
#include "nc_message.hpp"

std::vector<uint8_t> encode_message(std::vector<uint8_t> message, std::string secret_key) {
    std::vector<uint8_t> result;

    // 1. Compress message:

    // 2. Encrypt message:

    return result;
}

std::vector<uint8_t> decode_message(std::vector<uint8_t> message, std::string secret_key) {
    std::vector<uint8_t> result;

    // 1. Decrypt message:

    // 2. Decompress message:

    return result;
}
