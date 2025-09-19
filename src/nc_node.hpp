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
#include <mutex>
#include <atomic>

// Local includes:
#include "nc_config.hpp"
#include "nc_message.hpp"
#include "nc_network.hpp"

namespace NodeCrunch2 {
class NCNodeDataProcessor {
    public:
        // Default special member functions:
        NCNodeDataProcessor() = default;
        virtual ~NCNodeDataProcessor() = default;
        NCNodeDataProcessor (NCNodeDataProcessor&&) = default;
        NCNodeDataProcessor(const NCNodeDataProcessor&) = default;
        NCNodeDataProcessor& operator=(const NCNodeDataProcessor&) = default;
        NCNodeDataProcessor& operator=(NCNodeDataProcessor&&) = default;

        // Must be implemented by the user:
        virtual void nc_init(std::vector<uint8_t> data);
        [[nodiscard]] virtual std::vector<uint8_t> nc_process_data(std::vector<uint8_t> data);
};

class NCNode {
    public:
        // Constructor:
        NCNode(NCConfiguration config,
            NCNodeDataProcessor data_processor,
            NCMessageCodecNode const message_codec,
            NCNetworkClientBase const network_client);
        NCNode(NCConfiguration config,
            NCNodeDataProcessor data_processor,
            NCMessageCodecNode const message_codec);
        NCNode(NCConfiguration config,
            NCNodeDataProcessor data_processor,
            NCNetworkClientBase const network_client);
        NCNode(NCConfiguration config, NCNodeDataProcessor data_processor);

        // Disable all other special member functions:
        NCNode (NCNode&&) = delete;
        NCNode() = delete;
        NCNode(const NCNode&) = delete;
        NCNode& operator=(const NCNode&) = delete;
        NCNode& operator=(NCNode&&) = delete;

        // API:
        void nc_run();
        [[nodiscard]] NCNodeID nc_get_node_id();

    private:
        NCConfiguration config_intern;
        const NCNodeID node_id;
        std::atomic_bool quit;
        // TODO: make this configurable (max_error_count)
        uint8_t max_error_count;
        std::mutex node_mutex;
        NCMessageCodecNode message_codec_intern;
        NCNetworkClientBase network_client_intern;
        NCNodeDataProcessor data_processor_intern;

        [[nodiscard]] NCDecodedMessageFromServer nc_send_msg_return_answer(NCEncodedMessageToServer const& message);
        void nc_send_heartbeat();
};
}

#endif // FILE_NC_NODE_HPP_INCLUDED
