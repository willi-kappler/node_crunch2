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
#include <expected>

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

enum struct NCMessageError: uint8_t {
    NCCompressionError,
    NCDecompressionError,
    NCEncryptionError,
    NCDecryptionError
};

struct NCEncodedMessage {
    std::vector<uint8_t> data = {};
};

struct NCNodeMessage {
    std::vector<uint8_t> data = {};
};

struct NCServerMessage {
    std::vector<uint8_t> data = {};
};

void nc_to_big_endian_bytes(uint32_t value, std::vector<uint8_t> &bytes);

uint32_t nc_from_big_endian_bytes(std::vector<uint8_t> const& bytes);

std::expected<std::vector<uint8_t>, NCMessageError> nc_compress_message(std::vector<uint8_t> const& message);

std::expected<std::vector<uint8_t>, NCMessageError> nc_decompress_message(std::vector<uint8_t> const& message);

std::expected<std::vector<uint8_t>, NCMessageError> nc_encrypt_message(std::vector<uint8_t> const& message, std::string const& secret_key);

std::expected<std::vector<uint8_t>, NCMessageError> nc_decrypt_message(std::vector<uint8_t> const& message, std::string const &secret_key);

std::expected<NCEncodedMessage, NCMessageError> nc_encode_message(std::vector<uint8_t> const& message, std::string const& secret_key);

std::expected<std::vector<uint8_t>, NCMessageError> nc_decode_message(NCEncodedMessage const& message, std::string const& secret_key);

#endif // FILE_NC_MESSAGE_HPP_INCLUDED