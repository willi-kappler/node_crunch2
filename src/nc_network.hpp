/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines networking classes for the node and the server.
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

class NCNetworkSocketBase {
    public:
        virtual void nc_send_data(std::vector<uint8_t> const data);
        [[nodiscard]] virtual std::vector<uint8_t> nc_receive_data();
        [[nodiscard]] virtual std::string nc_address();
        
        // Default special member functions:
        NCNetworkSocketBase() = default;
        virtual ~NCNetworkSocketBase() = default;
        NCNetworkSocketBase(NCNetworkSocketBase&&) = default;
        NCNetworkSocketBase(const NCNetworkSocketBase&) = default;
        NCNetworkSocketBase& operator=(const NCNetworkSocketBase&) = default;
        NCNetworkSocketBase& operator=(NCNetworkSocketBase&&) = default;
};

class NCNetworkSocket: public NCNetworkSocketBase {
    public:
        void nc_send_data(std::vector<uint8_t> const data) override;
        [[nodiscard]] std::vector<uint8_t> nc_receive_data() override;
        [[nodiscard]] std::string nc_address() override;

        // Constructor:
        NCNetworkSocket(tcp::socket &socket);
        
        // Default special member functions:
        ~NCNetworkSocket() = default;
        NCNetworkSocket(NCNetworkSocket&&) = default;
        NCNetworkSocket(const NCNetworkSocket&) = default;
        NCNetworkSocket& operator=(const NCNetworkSocket&) = default;
        NCNetworkSocket& operator=(NCNetworkSocket&&) = default;

    private:
        tcp::socket socket_intern;
};

class NCNetworkClientBase {
    public:
        virtual NCNetworkSocketBase nc_connect();

        // Default special member functions:
        NCNetworkClientBase() = default;
        virtual ~NCNetworkClientBase() = default;
        NCNetworkClientBase(NCNetworkClientBase&&) = default;
        NCNetworkClientBase(const NCNetworkClientBase&) = default;
        NCNetworkClientBase& operator=(const NCNetworkClientBase&) = default;
        NCNetworkClientBase& operator=(NCNetworkClientBase&&) = default;
};

class NCNetworkClient: public NCNetworkClientBase {
    public:
        NCNetworkSocketBase nc_connect() override;
        //NCNetworkSocket nc_connect_to(std::string_view server, std::string_view port);

        // Constructor:
        NCNetworkClient(std::string_view server, uint16_t port);

        // Disable all other special member functions:
        NCNetworkClient(NCNetworkClient&&) = delete;
        NCNetworkClient(const NCNetworkClient&) = delete;
        NCNetworkClient& operator=(const NCNetworkClient&) = delete;
        NCNetworkClient& operator=(NCNetworkClient&&) = delete;

    private:
        asio::io_context io_context_intern;
        tcp::resolver resolver_intern;
        tcp::resolver::results_type endpoints_intern;
};

class NCNetworkServerBase {
    public:
        virtual NCNetworkSocketBase nc_accept();

        // Default special member functions:
        NCNetworkServerBase() = default;
        virtual ~NCNetworkServerBase() = default;
        NCNetworkServerBase(NCNetworkServerBase&&) = default;
        NCNetworkServerBase(const NCNetworkServerBase&) = default;
        NCNetworkServerBase& operator=(const NCNetworkServerBase&) = default;
        NCNetworkServerBase& operator=(NCNetworkServerBase&&) = default;
};

class NCNetworkServer: public NCNetworkServerBase {
    public:
        NCNetworkSocketBase nc_accept() override;

        // Constructor:
        NCNetworkServer(uint16_t server_port);

        // Disable all other special member functions:
        NCNetworkServer(NCNetworkServer&&) = delete;
        NCNetworkServer(const NCNetworkServer&) = delete;
        NCNetworkServer& operator=(const NCNetworkServer&) = delete;
        NCNetworkServer& operator=(NCNetworkServer&&) = delete;

    private:
        asio::io_context io_context_intern;
        tcp::acceptor acceptor_intern;
};

}

#endif // FILE_NC_NETWORK_HPP_INCLUDED
