/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines encryption of messages
*/

// STD includes:

// External includes:
//#include <botan/aead.h>
//#include <botan/auto_rng.h>
//#include <botan/secmem.h>

// Local includes:
#include "nc_encryption.hpp"

std::expected<NCEncodedMessage, NCMessageError> nc_encrypt_message(NCCompressedMessage const& message, std::string const& secret_key) {
    // TODO

    if (secret_key.size() == 0) {

    }

    return NCEncodedMessage(message.data);
}

std::expected<NCCompressedMessage, NCMessageError> nc_decrypt_message(NCEncodedMessage const& message, std::string const& secret_key) {
    // TODO

    if (secret_key.size() == 0) {

    }

    return NCCompressedMessage(message.data);
}
