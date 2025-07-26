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

[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_encode_message(NCRawMessage const& message, std::string const& secret_key);

[[nodiscard]] std::expected<NCDecodedMessage, NCMessageError> nc_decode_message(NCEncodedMessage const& message, std::string const& secret_key);

#endif // FILE_NC_MESSAGE_HPP_INCLUDED