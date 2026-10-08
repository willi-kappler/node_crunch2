/*
    Node Crunch2
    SPDX-License-Identifier: MIT
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
#include "nc_message_types.hpp"

namespace nodcru2 {
void nc_to_big_endian_bytes(const uint32_t, std::span<uint8_t>) noexcept;

[[nodiscard]] uint32_t nc_from_big_endian_bytes(const std::span<const uint8_t>);

[[nodiscard]] std::string nc_type_to_string(const NCNodeMessageType &);

[[nodiscard]] std::string nc_type_to_string(const NCServerMessageType &);

[[nodiscard]] std::string nc_gen_log_file_name(std::string_view);
}

#endif // FILE_NC_UTIL_HPP_INCLUDED
