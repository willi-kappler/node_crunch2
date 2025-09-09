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

// External includes:
#include <asio.hpp>

// Local includes:
//#include "nc_util.hpp"

using asio::ip::tcp;

namespace NodeCrunch2 {
void nc_send_data(std::vector<uint8_t> const data, tcp::socket& socket);

[[nodiscard]] std::vector<uint8_t> nc_receive_data(tcp::socket& socket);
}

#endif // FILE_NC_NETWORK_HPP_INCLUDED
