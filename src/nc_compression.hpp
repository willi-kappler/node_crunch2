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
        [[nodiscard]] virtual NCCompressedMessage nc_compress_message(NCDecompressedMessage const& message) const;
        [[nodiscard]] virtual NCDecompressedMessage nc_decompress_message(NCCompressedMessage const& message) const;

        // Constructor:
        NCCompressor() = default;

        // Destructor:
        virtual ~NCCompressor() = default;

        // Default special member functions:
        NCCompressor(NCCompressor&&) = default;
        NCCompressor(const NCCompressor&) = default;

        // Disable all other special member functions:
        NCCompressor& operator=(const NCCompressor&) = delete;
        NCCompressor& operator=(NCCompressor&&) = delete;
};

class NCNonCompressor: NCCompressor {
    public:
        [[nodiscard]] NCCompressedMessage nc_compress_message(NCDecompressedMessage const& message) const override;
        [[nodiscard]] NCDecompressedMessage nc_decompress_message(NCCompressedMessage const& message) const override;

        // Constructor:
        NCNonCompressor() = default;

        // Default special member functions:
        NCNonCompressor(NCNonCompressor&&) = default;
        NCNonCompressor(const NCNonCompressor&) = default;

        // Disable all other special member functions:
        NCNonCompressor& operator=(const NCNonCompressor&) = delete;
        NCNonCompressor& operator=(NCNonCompressor&&) = delete;

};

}

#endif // FILE_NC_COMPRESSION_HPP_INCLUDED