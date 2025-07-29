/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines encryption of messages
*/

#ifndef FILE_NC_ENCRYPTION_HPP_INCLUDED
#define FILE_NC_ENCRYPTION_HPP_INCLUDED

// STD includes:
#include <cstdint>
#include <vector>
#include <string>
#include <expected>

// Local includes:
#include "nc_message_errors.hpp"
#include "nc_message_types.hpp"

[[nodiscard]] std::expected<NCEncryptedMessage, NCMessageError> nc_encrypt_message(NCDecryptedMessage const& message, std::string const& secret_key);

[[nodiscard]] std::expected<NCDecryptedMessage, NCMessageError> nc_decrypt_message(NCEncryptedMessage const& message, std::string const &secret_key);

#endif // FILE_NC_ENCRYPTION_HPP_INCLUDED