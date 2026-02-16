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
#include <memory>

// Local includes:
#include "nc_message_types.hpp"
#include "nc_nodeid.hpp"
#include "nc_compression.hpp"
#include "nc_encryption.hpp"

namespace nodcru2 {
class NCMessageCodecBase {
    public:
        [[nodiscard]] virtual std::vector<uint8_t> nc_encode(NCDecompressedMessage decompressed_message) const;
        [[nodiscard]] virtual NCDecompressedMessage nc_decode(std::vector<uint8_t> const& message) const;

        // Constructor:
        NCMessageCodecBase(std::string const secret_key);
        NCMessageCodecBase(std::unique_ptr<NCCompressor> compressor,
            std::unique_ptr<NCEncryption> encryption);

        // Desctructor:
        virtual ~NCMessageCodecBase() = default;

        // Default special member functions:
        NCMessageCodecBase(const NCMessageCodecBase&) = delete;
        NCMessageCodecBase& operator=(const NCMessageCodecBase&) = default;
        NCMessageCodecBase(NCMessageCodecBase&&) = default;

        // Disable all other special member functions:
        NCMessageCodecBase& operator=(NCMessageCodecBase&&) = delete;

    private:
        std::unique_ptr<NCCompressor> compressor_intern;
        std::unique_ptr<NCEncryption> encryption_intern;
};

class NCMessageCodecNode: NCMessageCodecBase {
    public:
        [[nodiscard]] virtual NCEncodedMessageToServer nc_encode_message_to_server(
            NCNodeMessageType const msg_type, std::vector<uint8_t> const& data, NCNodeID const node_id) const;
        [[nodiscard]] virtual NCDecodedMessageFromServer nc_decode_message_from_server(
            NCEncodedMessageToNode const& message) const;

        [[nodiscard]] virtual NCEncodedMessageToServer nc_gen_heartbeat_message(NCNodeID const node_id) const;
        [[nodiscard]] virtual NCEncodedMessageToServer nc_gen_init_message(NCNodeID const node_id) const;
        [[nodiscard]] virtual NCEncodedMessageToServer nc_gen_result_message(
            std::vector<uint8_t> const& new_data, NCNodeID const node_id) const;
        [[nodiscard]] virtual NCEncodedMessageToServer nc_gen_need_more_data_message(NCNodeID const node_id) const;

        // Constructor:
        NCMessageCodecNode(std::string const secret_key);
        NCMessageCodecNode(std::unique_ptr<NCCompressor> compressor,
            std::unique_ptr<NCEncryption> encryption);

        // Desctructor:
        virtual ~NCMessageCodecNode() = default;

       // Default special member functions:
       NCMessageCodecNode(const NCMessageCodecNode&) = default;
       NCMessageCodecNode& operator=(const NCMessageCodecNode&) = default;
       NCMessageCodecNode(NCMessageCodecNode&&) = default;

       // Disable all other special member functions:
       NCMessageCodecNode& operator=(NCMessageCodecNode&&) = delete;
};

class NCMessageCodecServer: NCMessageCodecBase {
    public:
        [[nodiscard]] virtual NCEncodedMessageToNode nc_encode_message_to_node(NCServerMessageType const msg_type, std::vector<uint8_t> const& data) const;
        [[nodiscard]] virtual NCDecodedMessageFromNode nc_decode_message_from_node(NCEncodedMessageToServer const& message) const;

        [[nodiscard]] virtual NCEncodedMessageToNode nc_gen_heartbeat_message_ok() const;
        [[nodiscard]] virtual NCEncodedMessageToNode nc_gen_init_message_ok(std::vector<uint8_t> const& init_data) const;
        [[nodiscard]] virtual NCEncodedMessageToNode nc_gen_new_data_message(std::vector<uint8_t> const& new_data) const;
        [[nodiscard]] virtual NCEncodedMessageToNode nc_gen_result_ok_message() const;
        [[nodiscard]] virtual NCEncodedMessageToNode nc_gen_quit_message() const;
        [[nodiscard]] virtual NCEncodedMessageToNode nc_gen_invalid_node_id_error() const;
        [[nodiscard]] virtual NCEncodedMessageToNode nc_gen_unknown_error() const;

        // Constructor:
        NCMessageCodecServer(std::string const secret_key);
        NCMessageCodecServer(std::unique_ptr<NCCompressor> compressor,
            std::unique_ptr<NCEncryption> encryption);

        // Desctructor:
        virtual ~NCMessageCodecServer() = default;

        // Default special member functions:
        NCMessageCodecServer(const NCMessageCodecServer&) = default;
        NCMessageCodecServer& operator=(const NCMessageCodecServer&) = default;
        NCMessageCodecServer(NCMessageCodecServer&&) = default;

        // Disable all other special member functions:
        NCMessageCodecServer& operator=(NCMessageCodecServer&&) = delete;
};
}

#endif // FILE_NC_MESSAGE_HPP_INCLUDED
