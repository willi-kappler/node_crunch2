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
#include "nc_util.hpp"
#include "nc_message_errors.hpp"
#include "nc_message_types.hpp"

[[nodiscard]] std::expected<NCCompressedMessage, NCMessageError> nc_compress_message(NCDecompressedMessage const& message);

[[nodiscard]] std::expected<NCDecompressedMessage, NCMessageError> nc_decompress_message(NCCompressedMessage const& message);

#endif // FILE_NC_COMPRESSION_HPP_INCLUDED