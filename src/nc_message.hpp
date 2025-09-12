/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines the message type and decoding / encoding
*/

#ifndef FILE_NC_MESSAGE_HPP_INCLUDED
#define FILE_NC_MESSAGE_HPP_INCLUDED

// STD includes:
#include <cstdint>
#include <vector>
#include <string>
#include <string_view>
#include <expected>

// Local includes:
#include "nc_message_types.hpp"
#include "nc_nodeid.hpp"
#include "nc_compression.hpp"
#include "nc_encryption.hpp"

namespace NodeCrunch2 {
class NCMessageCodecBase {
    public:
        [[nodiscard]] std::vector<uint8_t> nc_encode(NCDecompressedMessage decompressed_message) const;
        [[nodiscard]] NCDecompressedMessage nc_decode(std::vector<uint8_t> const& message) const;

        // Constructor:
        NCMessageCodecBase(std::string const secret_key);
        NCMessageCodecBase(NCCompressor compressor, NCEncryption encryption);

        // Default special member functions:
        NCMessageCodecBase(NCMessageCodecBase&&) = default;

        // Disable all other special member functions:
        NCMessageCodecBase(const NCMessageCodecBase&) = delete;
        NCMessageCodecBase& operator=(const NCMessageCodecBase&) = delete;
        NCMessageCodecBase& operator=(NCMessageCodecBase&&) = delete;

    private:
        const NCCompressor compressor_intern;
        const NCEncryption encryption_intern;
};

class NCMessageCodecNode : NCMessageCodecBase {
    public:
        [[nodiscard]] NCEncodedMessageToServer nc_encode_message_to_server(NCNodeMessageType const msg_type, std::vector<uint8_t> const& data) const;
        [[nodiscard]] NCDecodedMessageFromServer nc_decode_message_from_server(NCEncodedMessageToNode const& message) const;

        [[nodiscard]] NCEncodedMessageToServer nc_gen_heartbeat_message() const;
        [[nodiscard]] NCEncodedMessageToServer nc_gen_init_message() const;
        [[nodiscard]] NCEncodedMessageToServer nc_gen_result_message(std::vector<uint8_t> const& new_data) const;
        [[nodiscard]] NCEncodedMessageToServer nc_gen_need_more_data_message() const;

        // Constructor:
        NCMessageCodecNode(NCNodeID const node_id, std::string const secret_key);
        NCMessageCodecNode(NCNodeID const node_id, NCCompressor compressor, NCEncryption encryption);

        // Default special member functions:
        NCMessageCodecNode(NCMessageCodecNode&&) = default;

        // Disable all other special member functions:
        NCMessageCodecNode(const NCMessageCodecNode&) = delete;
        NCMessageCodecNode& operator=(const NCMessageCodecNode&) = delete;
        NCMessageCodecNode& operator=(NCMessageCodecNode&&) = delete;

    private:
        const NCNodeID node_id_intern;
};

class NCMessageCodecServer : NCMessageCodecBase {
    public:
        [[nodiscard]] NCEncodedMessageToNode nc_encode_message_to_node(NCServerMessageType const msg_type, std::vector<uint8_t> const& data) const;
        [[nodiscard]] NCDecodedMessageFromNode nc_decode_message_from_node(NCEncodedMessageToServer const& message) const;

        [[nodiscard]] NCEncodedMessageToNode nc_gen_heartbeat_message_ok() const;
        [[nodiscard]] NCEncodedMessageToNode nc_gen_heartbeat_message_error() const;
        [[nodiscard]] NCEncodedMessageToNode nc_gen_init_message_ok(std::vector<uint8_t> const& init_data) const;
        [[nodiscard]] NCEncodedMessageToNode nc_gen_init_message_error() const;
        [[nodiscard]] NCEncodedMessageToNode nc_gen_new_data_message(std::vector<uint8_t> const& new_data) const;
        [[nodiscard]] NCEncodedMessageToNode nc_gen_result_ok_message() const;
        [[nodiscard]] NCEncodedMessageToNode nc_gen_quit_message() const;

        // Constructor:
        NCMessageCodecServer(std::string const secret_key);
        NCMessageCodecServer(NCCompressor compressor, NCEncryption encryption);

        // Default special member functions:
        NCMessageCodecServer(NCMessageCodecServer&&) = default;

        // Disable all other special member functions:
        NCMessageCodecServer(const NCMessageCodecServer&) = delete;
        NCMessageCodecServer& operator=(const NCMessageCodecServer&) = delete;
        NCMessageCodecServer& operator=(NCMessageCodecServer&&) = delete;
};
}

#endif // FILE_NC_MESSAGE_HPP_INCLUDED
