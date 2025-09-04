/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines some helper functions for networking.
*/

#ifndef FILE_NC_NETWORK_HPP_INCLUDED
#define FILE_NC_NETWORK_HPP_INCLUDED

// STD includes:
#include <cstdint>
#include <vector>
#include <expected>

// External includes:
#include <asio.hpp>

// Local includes:
#include "nc_util.hpp"
#include "nc_message_errors.hpp"

using asio::ip::tcp;

namespace NodeCrunch2 {
[[nodiscard]] std::expected<uint8_t, NCMessageError> nc_send_data(std::vector<uint8_t> const data, tcp::socket& socket);

[[nodiscard]] std::expected<std::vector<uint8_t>, NCMessageError> nc_receive_data(tcp::socket& socket);
}

#endif // FILE_NC_NETWORK_HPP_INCLUDED
