/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines some helper functions for networking.
*/

// External includes:
#include <spdlog/spdlog.h>

// Local includes:
#include "nc_util.hpp"
#include "nc_network.hpp"

namespace NodeCrunch2 {
void nc_send_data(std::vector<uint8_t> const data, tcp::socket& socket) {
    uint32_t data_size = static_cast<uint32_t>(data.size());
    std::array<uint8_t, 4> size_bytes;
    nc_to_big_endian_bytes(data_size, size_bytes);

    asio::write(socket, asio::buffer(size_bytes));
    asio::write(socket, asio::buffer(data));
}

[[nodiscard]] std::vector<uint8_t> nc_receive_data(tcp::socket& socket) {
    std::array<uint8_t, 4> size_bytes;

    asio::read(socket, asio::buffer(size_bytes));
    uint32_t data_size = nc_from_big_endian_bytes(size_bytes);

    std::vector<uint8_t> result(data_size);
    if (data_size > 0) {
        asio::read(socket, asio::buffer(result));
    }

    return result;
}
}
