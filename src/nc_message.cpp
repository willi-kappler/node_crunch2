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
    std::expected<NCDecompressedMessage, NCMessageError> decompressed_message = nc_decompress_message(*decrypted_message);
    if (!decompressed_message) {
        return std::unexpected(decompressed_message.error());
    }

    NCDecodedMessage result;
    result.data = decompressed_message->data;

    return result;
}

/*
[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_heartbeat_message(NCNodeID node_id, std::string secret_key) {

}

[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_heartbeat_message_ok(std::string secret_key) {

}

[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_heartbeat_message_error(std::string secret_key) {

}

[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_init_message(NCNodeID node_id, std::string secret_key) {

}

[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_init_message_ok(std::vector<uint8_t>, std::string secret_key) {

}

[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_init_message_error(std::string secret_key) {

}

[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_result_message(NCNodeID node_id, std::string secret_key, std::vector<uint8_t> new_data) {

}

[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_need_more_data_message(NCNodeID node_id, std::string secret_key) {

}

[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_new_data_message(std::vector<uint8_t> new_data, std::string secret_key) {

}

[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_result_ok_message(std::string secret_key) {

}

[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_quit_message(std::string secret_key) {

}
*/
