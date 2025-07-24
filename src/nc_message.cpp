/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines the message type and decoding / encoding
*/

// STD includes:
#include <cstring>

// Local includes:
#include "nc_compression.hpp"
#include "nc_encryption.hpp"
#include "nc_message.hpp"

std::expected<NCEncodedMessage, NCMessageError> nc_encode_message(NCRawMessage const& message, std::string const& secret_key) {
    // 1. Compress message:
    std::expected<NCCompressedMessage, NCMessageError> compressed_message = nc_compress_message(message);
    if (!compressed_message) {
        return std::unexpected(compressed_message.error());
    }

    // 2. Encrypt message:
    std::expected<NCEncodedMessage, NCMessageError> encrypted_message = nc_encrypt_message(*compressed_message, secret_key);
    if (!encrypted_message) {
        return std::unexpected(encrypted_message.error());
    }

    return *encrypted_message;
}

std::expected<NCDecodedMessage, NCMessageError> nc_decode_message(NCEncodedMessage const& message, std::string const& secret_key) {
    // 1. Decrypt message:
    std::expected<NCCompressedMessage, NCMessageError> decrypted_message = nc_decrypt_message(message, secret_key);
    if (!decrypted_message) {
        return std::unexpected(decrypted_message.error());
    }

    // 2. Decompress message:
    std::expected<NCDecodedMessage, NCMessageError> decompressed_message = nc_decompress_message(*decrypted_message);
    if (!decompressed_message) {
        return std::unexpected(decompressed_message.error());
    }

    return *decompressed_message;
}
