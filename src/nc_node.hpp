/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines the node class
*/

#ifndef FILE_NC_NODE_HPP_INCLUDED
#define FILE_NC_NODE_HPP_INCLUDED

// STD includes:
#include <cstdint>
#include <vector>
#include <string>
#include <expected>
#include <atomic>

// External includes:
#include <asio.hpp>

// Local includes:
#include "nc_config.hpp"
#include "nc_message.hpp"

using asio::ip::tcp;

class NCNode {
    public:
        // Constructor:
        NCNode(NCConfiguration config);

        // Destructor:
        virtual ~NCNode() = default;

        // Default special member functions:

        // Disable all other special member functions:
        NCNode (NCNode&&) = delete;
        NCNode() = delete;
        NCNode(const NCNode&) = delete;
        NCNode& operator=(const NCNode&) = delete;
        NCNode& operator=(NCNode&&) = delete;

        // API:
        void nc_run();

    private:
        std::string server_address;
        uint16_t server_port;
        uint16_t heartbeat_timeout;
        std::string secret_key;
        NCNodeID node_id;
        std::atomic_bool quit;
        // TODO: make this configurable:
        uint8_t max_error_count;

        [[nodiscard]] NCExpDecFromServer nc_send_msg_return_answer(NCExpEncToServer const& message,
            tcp::socket &socket, tcp::resolver::results_type &endpoints);
        void nc_send_heartbeat();

        // Must be implemented by the user:
        // (pure virtual functions)
        virtual void nc_init(std::vector<uint8_t> data) = 0;
        [[nodiscard]] virtual std::vector<uint8_t> nc_process_data(std::vector<uint8_t> data) = 0;
};

#endif // FILE_NC_NODE_HPP_INCLUDED
