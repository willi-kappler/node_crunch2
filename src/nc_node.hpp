/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines the node class
*/

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
        asio::io_context io_context;
        asio::ip::tcp::resolver resolver;
        asio::ip::tcp::resolver::results_type endpoints;
        asio::ip::tcp::socket socket;

        NCExpDecFromServer nc_send_msg_return_answer(NCExpEncToServer const& message);
        void nc_send_heartbeat();

        // Must be implemented by the user:
        // (pure virtual functions)
        virtual void nc_init(std::vector<uint8_t> data) = 0;
        virtual std::vector<uint8_t> nc_process_data(std::vector<uint8_t> data) = 0;
};
