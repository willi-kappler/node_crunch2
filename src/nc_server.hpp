/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines the server class
*/

#ifndef FILE_NC_SERVER_HPP_INCLUDED
#define FILE_NC_SERVER_HPP_INCLUDED

// STD includes:
#include <cstdint>
#include <vector>
#include <string>
#include <atomic>
#include <mutex>
#include <chrono>

// Local includes:
#include "nc_config.hpp"
#include "nc_nodeid.hpp"
#include "nc_message.hpp"
#include "nc_network.hpp"

namespace NodeCrunch2 {
class NCServer {
        // Constructor:
        NCServer(NCConfiguration config, NCMessageCodecServer const message_codec, NCNetworkServerBase const network_server);
        NCServer(NCConfiguration config, NCMessageCodecServer const message_codec);
        NCServer(NCConfiguration config, NCNetworkServerBase const network_server);
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
        NCConfiguration config_intern;
        std::atomic_bool quit;
        std::unordered_map<NCNodeID, std::chrono::time_point<std::chrono::steady_clock>> all_nodes;
        // In code use: const std::lock_guard<std::mutex> lock(server_mutex);
        std::mutex server_mutex;
        NCMessageCodecServer message_codec_intern;
        NCNetworkServerBase network_server_intern;

        void nc_register_new_node(NCNodeID node_id);
        void nc_update_node_time(NCNodeID node_id);
        void nc_handle_node(NCNetworkSocketBase &sock);
        void nc_check_heartbeat();
        bool nc_valid_node_id(NCNodeID node_id);

        // Must be implemented by the user:
        // (pure virtual functions)
        [[nodiscard]] virtual std::vector<uint8_t> nc_get_init_data() = 0;
        [[nodiscard]] virtual bool nc_is_job_done() = 0;
        virtual void nc_save_data() = 0;
        virtual void nc_node_timeout(NCNodeID node_id) = 0;
        [[nodiscard]] virtual std::vector<uint8_t> nc_get_new_data(NCNodeID node_id) = 0;
        virtual void nc_process_result(NCNodeID node_id, std::vector<uint8_t> result) = 0;
};
}

#endif // FILE_NC_SERVER_HPP_INCLUDED
