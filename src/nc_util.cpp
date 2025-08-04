/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines some helper functions.
*/

// Local includes:
#include "nc_util.hpp"

void nc_to_big_endian_bytes(uint32_t const value, std::span<uint8_t> bytes) noexcept {
    uint32_t final_value = value;

    if (std::endian::native == std::endian::little) {
        final_value = std::byteswap(value);
    }

    std::memcpy(bytes.data(), &final_value, sizeof(uint32_t));
}

uint32_t nc_from_big_endian_bytes(std::span<const uint8_t> const bytes) {
    uint32_t result;

    std::memcpy(&result, bytes.data(), sizeof(uint32_t));

    if (std::endian::native == std::endian::little) {
        result = std::byteswap(result);
    }

    return result;
}
