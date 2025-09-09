/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines compression of messages
*/

#ifndef FILE_NC_COMPRESSION_HPP_INCLUDED
#define FILE_NC_COMPRESSION_HPP_INCLUDED

// STD includes:
#include <cstdint>
#include <vector>
#include <string>
#include <expected>

// Local includes:
#include "nc_message_types.hpp"

namespace NodeCrunch2 {
class NCCompressor {
    public:
        [[nodiscard]] NCCompressedMessage nc_compress_message(NCDecompressedMessage const& message) const;

        [[nodiscard]] NCDecompressedMessage nc_decompress_message(NCCompressedMessage const& message) const;

        // Constructor:
        NCCompressor();

        // Default special member functions:
        NCCompressor (NCCompressor&&) = default;
        NCCompressor(const NCCompressor&) = default;

        // Disable all other special member functions:
        NCCompressor& operator=(const NCCompressor&) = delete;
        NCCompressor& operator=(NCCompressor&&) = delete;
};
}

#endif // FILE_NC_COMPRESSION_HPP_INCLUDED