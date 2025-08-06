/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines some helper functions.
*/

#ifndef FILE_NC_UTIL_HPP_INCLUDED
#define FILE_NC_UTIL_HPP_INCLUDED

// STD includes:
#include <cstdint>
#include <cstring>
#include <vector>
#include <bit>
#include <span>

// Local includes:
#include "nc_message_errors.hpp"
#include "nc_message_types.hpp"

void nc_to_big_endian_bytes(uint32_t const value, std::span<uint8_t> bytes) noexcept;

[[nodiscard]] uint32_t nc_from_big_endian_bytes(std::span<const uint8_t> const bytes);

[[nodiscard]] std::string nc_type_to_string(NCMessageType const& msg_type);

[[nodiscard]] std::string nc_error_to_str(NCMessageError const& msg_error);

#endif // FILE_NC_UTIL_HPP_INCLUDED
