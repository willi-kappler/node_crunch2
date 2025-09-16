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
void NCNetworkSocket::nc_send_data(std::vector<uint8_t> const data) {
    uint32_t data_size = static_cast<uint32_t>(data.size());
    std::array<uint8_t, 4> size_bytes;
    nc_to_big_endian_bytes(data_size, size_bytes);

    asio::write(socket_intern, asio::buffer(size_bytes));
    asio::write(socket_intern, asio::buffer(data));
}

[[nodiscard]] std::vector<uint8_t> NCNetworkSocket::nc_receive_data() {
    std::array<uint8_t, 4> size_bytes;

    asio::read(socket_intern, asio::buffer(size_bytes));
    uint32_t data_size = nc_from_big_endian_bytes(size_bytes);

    std::vector<uint8_t> result(data_size);
    if (data_size > 0) {
        asio::read(socket_intern, asio::buffer(result));
    }

    return result;
}

[[nodiscard]] std::string NCNetworkSocket::nc_address() {
    return socket_intern.remote_endpoint().address().to_string();
}

NCNetworkSocket::NCNetworkSocket(tcp::socket &socket): socket_intern(std::move(socket)) {}

NCNetworkSocket::NCNetworkSocket(): socket_intern([](){asio::io_context io_context_local; return tcp::socket(io_context_local); }()) {}


NCNetworkClient NCNetworkClient::clone(std::string_view server, uint16_t port) const {
    return NCNetworkClient(server, port);
}


NCNetworkSocket NCNetworkClient::nc_connect() {
    tcp::socket socket(io_context_intern);
    // tcp::endpoint connection = asio::connect(socket, endpoints_intern);
    asio::connect(socket, endpoints_intern);
    return NCNetworkSocket(socket);
}

NCNetworkClient::NCNetworkClient(std::string_view server, uint16_t port):
    io_context_intern(),
    resolver_intern(io_context_intern),
    endpoints_intern(resolver_intern.resolve(server, std::to_string(port)))
    {}

NCNetworkSocket NCNetworkServer::nc_accept() {
    tcp::socket socket(io_context_intern);
    acceptor_intern.accept(socket);
    return NCNetworkSocket(socket);
}

NCNetworkServer::NCNetworkServer(uint16_t server_port):
    io_context_intern(),
    acceptor_intern(io_context_intern, tcp::endpoint(tcp::v4(), server_port))
    {}
}
