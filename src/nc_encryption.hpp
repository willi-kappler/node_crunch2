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
#include "nc_message_types.hpp"

namespace NodeCrunch2 {
class NCEncryption {
    public:
        [[nodiscard]] NCEncryptedMessage nc_encrypt_message(NCDecryptedMessage const& message) const;

        [[nodiscard]] NCDecryptedMessage nc_decrypt_message(NCEncryptedMessage const& message) const;

        // Constructor:
        NCEncryption(std::string const secret_key);

        // Default special member functions:
        NCEncryption (NCEncryption&&) = default;
        NCEncryption(const NCEncryption&) = default;

        // Disable all other special member functions:
        NCEncryption& operator=(const NCEncryption&) = delete;
        NCEncryption& operator=(NCEncryption&&) = delete;

    private:
        const std::string secret_key;
};
}

#endif // FILE_NC_ENCRYPTION_HPP_INCLUDED