/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines the configuration options
*/

#ifndef FILE_NC_CONFIG_HPP_INCLUDED
#define FILE_NC_CONFIG_HPP_INCLUDED

// STD includes:
#include <string>
#include <cstdint>
#include <iostream>

// Local includes:
#include "nc_exceptions.hpp"

class NCConfiguration {
    public:
        std::string server_address;
        uint16_t server_port;
        uint16_t heartbeat_timeout;
        uint8_t quit_counter;
        std::string secret_key;

        NCConfiguration(std::string secret_key_user):
            // Member initialization list:
            server_address("127.0.0.1"),
            server_port(3100),
            heartbeat_timeout(60 * 5), // Seconds
            quit_counter(10), // Number of rounds to wait before quitting
            secret_key(secret_key_user)
        {
            size_t key_length = secret_key_user.size();
            if (key_length != 32)
            {
                std::cerr << "Secret key must be exactly 32 bytes long, but has " << key_length << " chars.\n";
                // std::exit(EXIT_FAILURE);
                throw NCInvalidKeyException();
            }

            // TODO: convert string to array of bytes.
            secret_key = secret_key_user;
        }

        // Default special member functions:
        NCConfiguration (NCConfiguration&&) = default;

        // Disable all other special member functions:
        NCConfiguration() = delete;
        NCConfiguration(const NCConfiguration&) = delete;
        NCConfiguration& operator=(const NCConfiguration&) = delete;
        NCConfiguration& operator=(NCConfiguration&&) = delete;
};

NCConfiguration nc_config_from_file(std::string_view);

#endif // FILE_NC_CONFIG_HPP_INCLUDED