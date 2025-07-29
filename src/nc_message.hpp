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
#include <expected>

// Local includes:
#include "nc_message_errors.hpp"
#include "nc_message_types.hpp"
#include "nc_nodeid.hpp"

[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_encode_message(NCMessageType const msg_type, std::string const& node_id, std::vector<uint8_t> const& data, std::string const& secret_key);
[[nodiscard]] std::expected<NCDecodedMessage, NCMessageError> nc_decode_message(NCEncodedMessage const& message, std::string const& secret_key);
/*
[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_heartbeat_message(NCNodeID node_id, std::string secret_key);
[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_heartbeat_message_ok(std::string secret_key);
[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_heartbeat_message_error(std::string secret_key);
[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_init_message(NCNodeID node_id, std::string secret_key);
[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_init_message_ok(std::vector<uint8_t>, std::string secret_key);
[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_init_message_error(std::string secret_key);
[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_result_message(NCNodeID node_id, std::string secret_key, std::vector<uint8_t> new_data);
[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_need_more_data_message(NCNodeID node_id, std::string secret_key);
[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_new_data_message(std::vector<uint8_t> new_data, std::string secret_key);
[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_result_ok_message(std::string secret_key);
[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_quit_message(std::string secret_key);
*/

#endif // FILE_NC_MESSAGE_HPP_INCLUDED