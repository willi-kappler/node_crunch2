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

// Local includes:
#include "nc_config.hpp"
#include "nc_nodeid.hpp"
#include "nc_message.hpp"
#include "nc_network.hpp"

namespace NodeCrunch2 {
class NCServerDataProcessor {
    public:
        // Default special member functions:
        NCServerDataProcessor() = default;
        virtual ~NCServerDataProcessor() = default;
        NCServerDataProcessor (NCServerDataProcessor&&) = default;
        NCServerDataProcessor(const NCServerDataProcessor&) = default;
        NCServerDataProcessor& operator=(const NCServerDataProcessor&) = default;
        NCServerDataProcessor& operator=(NCServerDataProcessor&&) = default;

        // Must be implemented by the user:
        [[nodiscard]] virtual std::vector<uint8_t> nc_get_init_data();
        [[nodiscard]] virtual bool nc_is_job_done();
        virtual void nc_save_data();
        virtual void nc_node_timeout(NCNodeID node_id);
        [[nodiscard]] virtual std::vector<uint8_t> nc_get_new_data(NCNodeID node_id);
        virtual void nc_process_result(NCNodeID node_id, std::vector<uint8_t> result);
};

class NCServer {
    public:
        // Constructor:
        NCServer(NCConfiguration config,
            std::shared_ptr<NCServerDataProcessor> data_processor,
            std::unique_ptr<NCMessageCodecServer> message_codec,
            std::unique_ptr<NCNetworkServerBase> network_server);
        NCServer(NCConfiguration config,
            std::shared_ptr<NCServerDataProcessor> data_processor,
            std::unique_ptr<NCMessageCodecServer> message_codec);
        NCServer(NCConfiguration config,
            std::shared_ptr<NCServerDataProcessor> data_processor,
            std::unique_ptr<NCNetworkServerBase> network_server);
        NCServer(NCConfiguration config,
            std::shared_ptr<NCServerDataProcessor> data_processor);

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
        std::mutex server_mutex;
        std::unique_ptr<NCMessageCodecServer> message_codec_intern;
        std::unique_ptr<NCNetworkServerBase> network_server_intern;
        std::shared_ptr<NCServerDataProcessor> data_processor_intern;

        void nc_register_new_node(NCNodeID node_id);
        void nc_update_node_time(NCNodeID node_id);
        void nc_handle_node(std::unique_ptr<NCNetworkSocketBase> &sock);
        void nc_check_heartbeat();
        bool nc_valid_node_id(NCNodeID node_id);
};
}

#endif // FILE_NC_SERVER_HPP_INCLUDED
