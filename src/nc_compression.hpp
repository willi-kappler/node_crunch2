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
#include "nc_message_errors.hpp"
#include "nc_message_types.hpp"

void nc_to_big_endian_bytes(uint32_t const value, std::vector<uint8_t> &bytes) noexcept;

[[nodiscard]] uint32_t nc_from_big_endian_bytes(std::vector<uint8_t> const& bytes);

[[nodiscard]] std::expected<NCCompressedMessage, NCMessageError> nc_compress_message(NCRawMessage const& message);

[[nodiscard]] std::expected<NCDecompressedMessage, NCMessageError> nc_decompress_message(NCCompressedMessage const& message);

#endif // FILE_NC_COMPRESSION_HPP_INCLUDED