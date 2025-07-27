/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines compresseion of messages
*/

// STD includes:
#include <cstring>
#include <bit>

// External includes:
#include <lz4.h>

// Local includes:
#include "nc_compression.hpp"

void nc_to_big_endian_bytes(uint32_t const value, std::vector<uint8_t> &bytes) noexcept {
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

std::expected<NCCompressedMessage, NCMessageError> nc_compress_message(NCRawMessage const& message) {
    const uint32_t original_size = static_cast<uint32_t>(message.data.size());
    const uint32_t max_compressed_size = LZ4_compressBound(original_size);
    std::vector<uint8_t> compressed_data(max_compressed_size + 4);

    const int32_t compressed_size = LZ4_compress_default(
        reinterpret_cast<const char*>(message.data.data()),
        reinterpret_cast<char*>(compressed_data.data() + 4),
        static_cast<int>(original_size),
        max_compressed_size
    );

    if (compressed_size <= 0) {
        return std::unexpected(NCMessageError::NCCompressionError);
    } else {
        compressed_data.resize(compressed_size + 4);
        nc_to_big_endian_bytes(original_size, compressed_data);
        return NCCompressedMessage(compressed_data);
    }
}

std::expected<NCDecodedMessage, NCMessageError> nc_decompress_message(NCCompressedMessage const& message) {
    const uint32_t original_size = nc_from_big_endian_bytes(message.data);
    std::vector<uint8_t> decompressed_data(original_size);
    const int32_t decompressed_size = LZ4_decompress_safe(
        reinterpret_cast<const char*>(message.data.data() + 4),
        reinterpret_cast<char*>(decompressed_data.data()),
        static_cast<int>(message.data.size() - 4),
        original_size
    );

    if (decompressed_size <= 0) {
        return std::unexpected(NCMessageError::NCDecompressionError);
    }

    return NCDecodedMessage(decompressed_data);
}
