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
#include "nc_message_errors.hpp"
#include "nc_message_types.hpp"
#include "nc_nodeid.hpp"
#include "nc_compression.hpp"
#include "nc_encryption.hpp"

namespace NodeCrunch2 {
using NCExpEncToServer = std::expected<NCEncodedMessageToServer, NCMessageError>;
using NCExpEncToNode = std::expected<NCEncodedMessageToNode, NCMessageError>;
using NCExpDecFromServer = std::expected<NCDecodedMessageFromServer, NCMessageError>;
using NCExpDecFromNode = std::expected<NCDecodedMessageFromNode, NCMessageError>;

class NCMessageCodecBase {
    public:
        [[nodiscard]] std::vector<uint8_t> nc_encode(NCDecompressedMessage decompressed_message);
        [[nodiscard]] NCDecompressedMessage nc_decode(std::vector<uint8_t> const& message);

        // Constructor:
        NCMessageCodecBase(std::string const secret_key);
        NCMessageCodecBase(NCCompressor nc_compressor, NCEncryption nc_encryption);

        // Default special member functions:
        NCMessageCodecBase (NCMessageCodecBase&&) = default;

        // Disable all other special member functions:
        NCMessageCodecBase(const NCMessageCodecBase&) = delete;
        NCMessageCodecBase& operator=(const NCMessageCodecBase&) = delete;
        NCMessageCodecBase& operator=(NCMessageCodecBase&&) = delete;

    private:
        NCCompressor nc_compressor;
        NCEncryption nc_encryption;
};

class NCMessageCodecNode : NCMessageCodecBase {
    public:
        [[nodiscard]] NCEncodedMessageToServer nc_encode_message_to_server(NCMessageType const msg_type, std::vector<uint8_t> const& data);
        [[nodiscard]] NCDecodedMessageFromServer nc_decode_message_from_server(NCEncodedMessageToNode const& message);

        [[nodiscard]] NCEncodedMessageToServer nc_gen_heartbeat_message();
        [[nodiscard]] NCEncodedMessageToServer nc_gen_init_message();
        [[nodiscard]] NCEncodedMessageToServer nc_gen_result_message(std::vector<uint8_t> const& new_data);
        [[nodiscard]] NCEncodedMessageToServer nc_gen_need_more_data_message();

        // Constructor:
        NCMessageCodecNode(NCNodeID const node_id2, std::string const secret_key);
        NCMessageCodecNode(NCNodeID const node_id2, NCCompressor nc_compressor, NCEncryption nc_encryption);

        // Default special member functions:
        NCMessageCodecNode (NCMessageCodecNode&&) = default;

        // Disable all other special member functions:
        NCMessageCodecNode(const NCMessageCodecNode&) = delete;
        NCMessageCodecNode& operator=(const NCMessageCodecNode&) = delete;
        NCMessageCodecNode& operator=(NCMessageCodecNode&&) = delete;

    private:
        NCNodeID const node_id;
};

class NCMessageCodecServer : NCMessageCodecBase {
    public:
        [[nodiscard]] NCEncodedMessageToNode nc_encode_message_to_node(NCMessageType const msg_type, std::vector<uint8_t> const& data);
        [[nodiscard]] NCDecodedMessageFromNode nc_decode_message_from_node(NCEncodedMessageToServer const& message);

        [[nodiscard]] NCExpEncToNode nc_gen_heartbeat_message_ok();
        [[nodiscard]] NCExpEncToNode nc_gen_heartbeat_message_error();
        [[nodiscard]] NCExpEncToNode nc_gen_init_message_ok(std::vector<uint8_t> const& init_data);
        [[nodiscard]] NCExpEncToNode nc_gen_init_message_error();
        [[nodiscard]] NCExpEncToNode nc_gen_new_data_message(std::vector<uint8_t> const& new_data);
        [[nodiscard]] NCExpEncToNode nc_gen_result_ok_message();
        [[nodiscard]] NCExpEncToNode nc_gen_quit_message();

        // Constructor:
        NCMessageCodecServer(std::string const secret_key);
        NCMessageCodecServer(NCCompressor nc_compressor, NCEncryption nc_encryption);

        // Default special member functions:
        NCMessageCodecServer (NCMessageCodecServer&&) = default;

        // Disable all other special member functions:
        NCMessageCodecServer(const NCMessageCodecServer&) = delete;
        NCMessageCodecServer& operator=(const NCMessageCodecServer&) = delete;
        NCMessageCodecServer& operator=(NCMessageCodecServer&&) = delete;
};

template <typename RetType>
[[nodiscard]] std::expected<RetType, NCMessageError> nc_encode2(NCMessageType const msg_type,
        std::string_view node_id, std::vector<uint8_t> const& data, std::string const& secret_key);

template <typename RetType>
[[nodiscard]] std::expected<RetType, NCMessageError> nc_decode2(std::vector<uint8_t> const& message,
        std::string const& secret_key);

[[nodiscard]] NCExpEncToServer nc_encode_message_to_server2(NCMessageType const msg_type,
    NCNodeID const& node_id, std::vector<uint8_t> const& data, std::string const& secret_key);

[[nodiscard]] NCExpEncToNode nc_encode_message_to_node2(NCMessageType const msg_type,
    std::vector<uint8_t> const& data, std::string const& secret_key);

[[nodiscard]] NCExpDecFromServer nc_decode_message_from_server2(NCEncodedMessageToNode const& message, std::string const& secret_key);

[[nodiscard]] NCExpDecFromNode nc_decode_message_from_node2(NCEncodedMessageToServer const& message, std::string const& secret_key);

[[nodiscard]] NCExpEncToServer nc_gen_heartbeat_message2(NCNodeID const& node_id, std::string const& secret_key);
[[nodiscard]] NCExpEncToNode nc_gen_heartbeat_message_ok2(std::string const& secret_key);
[[nodiscard]] NCExpEncToNode nc_gen_heartbeat_message_error2(std::string const& secret_key);
[[nodiscard]] NCExpEncToServer nc_gen_init_message2(NCNodeID const& node_id, std::string const& secret_key);
[[nodiscard]] NCExpEncToNode nc_gen_init_message_ok2(std::vector<uint8_t> const& init_data, std::string const& secret_key);
[[nodiscard]] NCExpEncToNode nc_gen_init_message_error2(std::string const& secret_key);
[[nodiscard]] NCExpEncToServer nc_gen_result_message2(NCNodeID const& node_id,
    std::vector<uint8_t> const& new_data, std::string const& secret_key);
[[nodiscard]] NCExpEncToServer nc_gen_need_more_data_message2(NCNodeID const& node_id, std::string const& secret_key);
[[nodiscard]] NCExpEncToNode nc_gen_new_data_message2(std::vector<uint8_t> const& new_data, std::string const& secret_key);
[[nodiscard]] NCExpEncToNode nc_gen_result_ok_message2(std::string const& secret_key);
[[nodiscard]] NCExpEncToNode nc_gen_quit_message2(std::string const& secret_key);
}

#endif // FILE_NC_MESSAGE_HPP_INCLUDED
