/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines compresseion of messages
*/

// External includes:
#include <lz4.h>

// Local includes:
#include "nc_compression.hpp"
#include "nc_util.hpp"
#include "nc_exceptions.hpp"

namespace NodeCrunch2 {
[[nodiscard]] NCCompressedMessage NCCompressor::nc_compress_message(NCDecompressedMessage const& message) const {
    const uint32_t original_size = static_cast<uint32_t>(message.data.size());
    const size_t max_compressed_size = static_cast<size_t>(LZ4_compressBound(static_cast<int>(original_size)));
    std::vector<uint8_t> compressed_data(max_compressed_size + 4);

    const size_t compressed_size = static_cast<size_t>(LZ4_compress_default(
        reinterpret_cast<const char*>(message.data.data()),
        reinterpret_cast<char*>(compressed_data.data() + 4),
        static_cast<int>(original_size),
        static_cast<int>(max_compressed_size)
    ));

    if (compressed_size > 0) {
        compressed_data.resize(compressed_size + 4);
        nc_to_big_endian_bytes(original_size, compressed_data);
        return NCCompressedMessage(compressed_data);
    } else {
        throw NCCompressionException();
    }
}

[[nodiscard]] NCDecompressedMessage NCCompressor::nc_decompress_message(NCCompressedMessage const& message) const {
    const uint32_t original_size = nc_from_big_endian_bytes(message.data);
    std::vector<uint8_t> decompressed_data(original_size);
    const int32_t decompressed_size = LZ4_decompress_safe(
        reinterpret_cast<const char*>(message.data.data() + 4),
        reinterpret_cast<char*>(decompressed_data.data()),
        static_cast<int>(message.data.size() - 4),
        static_cast<int>(original_size)
    );

    if (decompressed_size > 0) {
        return NCDecompressedMessage(decompressed_data);
    } else {
        throw NCDecompressionException();
    }
}

[[nodiscard]] NCCompressedMessage NCNonCompressor::nc_compress_message(NCDecompressedMessage const& message) const {
    const uint32_t original_size = static_cast<uint32_t>(message.data.size());
    std::vector<uint8_t> compressed_data(original_size + 4);
    std::copy(message.data.cbegin(), message.data.cend(), compressed_data.begin() + 4);
    nc_to_big_endian_bytes(original_size, compressed_data);
    return NCCompressedMessage(compressed_data);
}

[[nodiscard]] NCDecompressedMessage NCNonCompressor::nc_decompress_message(NCCompressedMessage const& message) const {
    const uint32_t original_size = nc_from_big_endian_bytes(message.data);
    std::vector<uint8_t> decompressed_data(original_size);
    std::copy(message.data.cbegin() + 4, message.data.cend(), decompressed_data.begin());
    return NCDecompressedMessage{decompressed_data};
}

}
