/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines the message type and decoding / encoding
*/

// STD includes:
#include <cstring>
#include <bit>

// External includes:
#include <lz4.h>

// Local includes:
#include "nc_message.hpp"

void nc_to_big_endian_bytes(uint32_t value, std::vector<uint8_t> &bytes) {
    uint32_t final_value = value;

    if (std::endian::native == std::endian::little) {
        final_value = std::byteswap(value);
    }

    std::memcpy(bytes.data(), &final_value, sizeof(uint32_t));
}

uint32_t nc_from_big_endian_bytes(std::vector<uint8_t> const& bytes) {
    uint32_t result;

    std::memcpy(&result, bytes.data(), sizeof(uint32_t));

    if (std::endian::native == std::endian::little) {
        result = std::byteswap(result);
    }

    return result;
}

std::expected<std::vector<uint8_t>, NCMessageError> nc_compress_message(std::vector<uint8_t> const& message) {
    const uint32_t original_size = static_cast<uint32_t>(message.size());
    const uint32_t max_compressed_size = LZ4_compressBound(original_size);
    std::vector<uint8_t> compressed_data(max_compressed_size + 4);

    const int32_t compressed_size = LZ4_compress_default(
        reinterpret_cast<const char*>(message.data()),
        reinterpret_cast<char*>(compressed_data.data() + 4),
        static_cast<int>(message.size()),
        max_compressed_size
    );

    if (compressed_size <= 0) {
        return std::unexpected(NCMessageError::NCCompressionError);
    } else {
        compressed_data.resize(compressed_size + 4);
        nc_to_big_endian_bytes(original_size, compressed_data);
        return compressed_data;
    }
}

std::expected<std::vector<uint8_t>, NCMessageError> nc_decompress_message(std::vector<uint8_t> const& message) {
    const uint32_t original_size = nc_from_big_endian_bytes(message);
    std::vector<uint8_t> decompressed_data(original_size);
    const int32_t decompressed_size = LZ4_decompress_safe(
        reinterpret_cast<const char*>(message.data() + 4),
        reinterpret_cast<char*>(decompressed_data.data()),
        static_cast<int>(message.size() - 4),
        original_size
    );

    if (decompressed_size <= 0) {
        return std::unexpected(NCMessageError::NCDecompressionError);
    }

    return decompressed_data;
}

std::expected<std::vector<uint8_t>, NCMessageError> nc_encrypt_message(std::vector<uint8_t> const& message, std::string const& secret_key) {
    // TODO

    if (secret_key.size() == 0) {

    }

    return message;
}

std::expected<std::vector<uint8_t>, NCMessageError> nc_decrypt_message(std::vector<uint8_t> const& message, std::string const& secret_key) {
    // TODO

    if (secret_key.size() == 0) {

    }

    return message;
}

std::expected<NCEncodedMessage, NCMessageError> nc_encode_message(std::vector<uint8_t> const& message, std::string const& secret_key) {
    NCEncodedMessage result;

    // 1. Compress message:
    std::expected<std::vector<uint8_t>, NCMessageError> compressed_message = nc_compress_message(message);
    if (!compressed_message) {
        return std::unexpected(compressed_message.error());
    }

    // 2. Encrypt message:
    std::expected<std::vector<uint8_t>, NCMessageError> encrypted_message = nc_encrypt_message(*compressed_message, secret_key);
    if (!encrypted_message) {
        return std::unexpected(encrypted_message.error());
    }
    result.data = *encrypted_message;

    return result;
}

std::expected<std::vector<uint8_t>, NCMessageError> nc_decode_message(NCEncodedMessage const& message, std::string const& secret_key) {
    // 1. Decrypt message:
    std::expected<std::vector<uint8_t>, NCMessageError> decrypted_message = nc_decrypt_message(message.data, secret_key);
    if (!decrypted_message) {
        return std::unexpected(decrypted_message.error());
    }

    // 2. Decompress message:
    std::expected<std::vector<uint8_t>, NCMessageError> decompressed_message = nc_decompress_message(*decrypted_message);
    if (!decompressed_message) {
        return std::unexpected(decompressed_message.error());
    }

    return *decompressed_message;
}
