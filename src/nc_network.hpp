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

namespace NodeCrunch2 {
using asio::ip::tcp;

void nc_send_data(std::vector<uint8_t> const data, tcp::socket& socket);
[[nodiscard]] std::vector<uint8_t> nc_receive_data(tcp::socket& socket);

class NCNetworkSocket {
    public:
        virtual void nc_send_data(std::vector<uint8_t> const data);
        [[nodiscard]] virtual std::vector<uint8_t> nc_receive_data();
        [[nodiscard]] virtual std::string nc_address();

        // Constructor:
        NCNetworkSocket();
        NCNetworkSocket(tcp::socket &socket);

        // Destructor:
        virtual ~NCNetworkSocket() = default;

        // Default special member functions:
        NCNetworkSocket(NCNetworkSocket&&) = default;
        NCNetworkSocket(const NCNetworkSocket&) = default;
        NCNetworkSocket& operator=(const NCNetworkSocket&) = default;
        NCNetworkSocket& operator=(NCNetworkSocket&&) = default;

        // Disable all other special member functions:

    private:
        tcp::socket socket_intern;
};

class NCNetworkClient {
    public:
        virtual NCNetworkSocket nc_connect();
        //virtual NCNetworkSocket nc_connect_to(std::string_view server, std::string_view port);

        // Constructor:
        NCNetworkClient(std::string_view server, uint16_t port);

        // Destructor:
        virtual ~NCNetworkClient() = default;

        // Default special member functions:
        NCNetworkClient(NCNetworkClient&&) = default;
        NCNetworkClient(const NCNetworkClient&) = default;

        // Disable all other special member functions:
        NCNetworkClient& operator=(const NCNetworkClient&) = delete;
        NCNetworkClient& operator=(NCNetworkClient&&) = delete;

    private:
        asio::io_context io_context_intern;
        tcp::resolver resolver_intern;
        tcp::resolver::results_type endpoints_intern;
};

class NCNetworkServer {
    public:
        virtual NCNetworkSocket nc_accept();

        // Constructor:
        NCNetworkServer(uint16_t server_port);

        // Destructor:
        virtual ~NCNetworkServer() = default;

        // Default special member functions:
        NCNetworkServer(NCNetworkServer&&) = default;
        NCNetworkServer(const NCNetworkServer&) = default;

        // Disable all other special member functions:
        NCNetworkServer& operator=(const NCNetworkServer&) = delete;
        NCNetworkServer& operator=(NCNetworkServer&&) = delete;

    private:
        asio::io_context io_context_intern;
        tcp::acceptor acceptor_intern;
};

}

#endif // FILE_NC_NETWORK_HPP_INCLUDED
