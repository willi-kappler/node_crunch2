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

void nc_to_big_endian_bytes(uint32_t const value, std::span<uint8_t> bytes) noexcept;

[[nodiscard]] uint32_t nc_from_big_endian_bytes(std::span<const uint8_t> const bytes);

#endif // FILE_NC_UTIL_HPP_INCLUDED
