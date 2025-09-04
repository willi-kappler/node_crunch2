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

namespace NodeCrunch2 {
using NCExpEncToServer = std::expected<NCEncodedMessageToServer, NCMessageError>;
using NCExpEncToNode = std::expected<NCEncodedMessageToNode, NCMessageError>;
using NCExpDecFromServer = std::expected<NCDecodedMessageFromServer, NCMessageError>;
using NCExpDecFromNode = std::expected<NCDecodedMessageFromNode, NCMessageError>;

template <typename RetType>
[[nodiscard]] std::expected<RetType, NCMessageError> nc_encode(NCMessageType const msg_type,
        std::string_view node_id, std::vector<uint8_t> const& data, std::string const& secret_key);

template <typename RetType>
[[nodiscard]] std::expected<RetType, NCMessageError> nc_decode(std::vector<uint8_t> const& message,
        std::string const& secret_key);

[[nodiscard]] NCExpEncToServer nc_encode_message_to_server(NCMessageType const msg_type,
    NCNodeID const& node_id, std::vector<uint8_t> const& data, std::string const& secret_key);

[[nodiscard]] NCExpEncToNode nc_encode_message_to_node(NCMessageType const msg_type,
    std::vector<uint8_t> const& data, std::string const& secret_key);

[[nodiscard]] NCExpDecFromServer nc_decode_message_from_server(NCEncodedMessageToNode const& message, std::string const& secret_key);

[[nodiscard]] NCExpDecFromNode nc_decode_message_from_node(NCEncodedMessageToServer const& message, std::string const& secret_key);

[[nodiscard]] NCExpEncToServer nc_gen_heartbeat_message(NCNodeID const& node_id, std::string const& secret_key);
[[nodiscard]] NCExpEncToNode nc_gen_heartbeat_message_ok(std::string const& secret_key);
[[nodiscard]] NCExpEncToNode nc_gen_heartbeat_message_error(std::string const& secret_key);
[[nodiscard]] NCExpEncToServer nc_gen_init_message(NCNodeID const& node_id, std::string const& secret_key);
[[nodiscard]] NCExpEncToNode nc_gen_init_message_ok(std::vector<uint8_t> const& init_data, std::string const& secret_key);
[[nodiscard]] NCExpEncToNode nc_gen_init_message_error(std::string const& secret_key);
[[nodiscard]] NCExpEncToServer nc_gen_result_message(NCNodeID const& node_id,
    std::vector<uint8_t> const& new_data, std::string const& secret_key);
[[nodiscard]] NCExpEncToServer nc_gen_need_more_data_message(NCNodeID const& node_id, std::string const& secret_key);
[[nodiscard]] NCExpEncToNode nc_gen_new_data_message(std::vector<uint8_t> const& new_data, std::string const& secret_key);
[[nodiscard]] NCExpEncToNode nc_gen_result_ok_message(std::string const& secret_key);
[[nodiscard]] NCExpEncToNode nc_gen_quit_message(std::string const& secret_key);
}

#endif // FILE_NC_MESSAGE_HPP_INCLUDED