/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines the server class
*/

#ifndef FILE_NC_MESSAGE_HPP_INCLUDED
#define FILE_NC_MESSAGE_HPP_INCLUDED

// STD includes:
#include <cstdint>
#include <vector>
#include <string>
#include <atomic>
#include <mutex>
#include <chrono>
//#include <expected>

// External includes:
// #include <asio.hpp>

// Local includes:
#include "nc_config.hpp"
#include "nc_nodeid.hpp"
#include "nc_message.hpp"

//using asio::ip::tcp;

class NCServer {
        // Constructor:
        NCServer(NCConfiguration config);

        // Destructor:
        virtual ~NCServer() = default;

        // Default special member functions:

        // Disable all other special member functions:
        NCServer (NCServer&&) = delete;
        NCServer() = delete;
        NCServer(const NCServer&) = delete;
        NCServer& operator=(const NCServer&) = delete;
        NCServer& operator=(NCServer&&) = delete;

        // API:
        void nc_run();

    private:
        std::string server_address;
        uint16_t server_port;
        uint16_t heartbeat_timeout;
        std::string secret_key;
        std::atomic_bool quit;
        uint8_t quit_counter;
        std::unordered_map<NCNodeID, std::chrono::time_point<std::chrono::steady_clock>> all_nodes;
        // In code use: const std::lock_guard<std::mutex> lock(server_mutex);
        std::mutex server_mutex;

        void nc_register_new_node(NCNodeID node_id);
        void nc_update_node_time(NCNodeID node_id);
        void nc_handle_node();
        void check_heartbeat();

        // Must be implemented by the user:
        // (pure virtual functions)
        [[nodiscard]] virtual std::vector<uint8_t> nc_get_init_data() = 0;
        [[nodiscard]] virtual bool nc_is_job_done() = 0;
        virtual void nc_save_data() = 0;
        virtual void nc_node_timeout(NCNodeID node_id) = 0;
        [[nodiscard]] virtual std::vector<uint8_t> nc_get_new_data(NCNodeID node_id) = 0;
        virtual void nc_process_result(NCNodeID node_id, std::vector<uint8_t> result) = 0;
};

#endif // FILE_NC_SERVER_HPP_INCLUDED
