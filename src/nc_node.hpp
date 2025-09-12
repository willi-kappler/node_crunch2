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
class NCNode {
    public:
        // Constructor:
        NCNode(NCConfiguration config);
        // TODO: NCNode(NCConfiguration config, NCMessageCodedNode nc_message_codec);

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
        NCConfiguration config_intern;
        NCNodeID node_id;
        std::atomic_bool quit;
        // TODO: make this configurable:
        uint8_t max_error_count;
        NCMessageCodecNode message_codec_intern;
        NCNetworkClient network_client_intern;
        std::mutex node_mutex;

        [[nodiscard]] NCDecodedMessageFromServer nc_send_msg_return_answer(NCEncodedMessageToServer const& message);
        void nc_send_heartbeat();

        // Must be implemented by the user:
        // (pure virtual functions)
        virtual void nc_init(std::vector<uint8_t> data) = 0;
        [[nodiscard]] virtual std::vector<uint8_t> nc_process_data(std::vector<uint8_t> data) = 0;
};
}

#endif // FILE_NC_NODE_HPP_INCLUDED
