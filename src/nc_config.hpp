/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines the configuration options
*/

#ifndef FILE_NC_CONFIG_HPP_INCLUDED
#define FILE_NC_CONFIG_HPP_INCLUDED

#include <string>
#include <cstdint>
#include <array>
#include <cstdlib>
#include <iostream>

class NCConfiguration {
    public:
        std::string server_address;
        uint16_t server_port;
        uint16_t heartbeat_timeout;
        uint8_t quit_counter;
        std::array<uint8_t, 32> secret_key;

        NCConfiguration(std::string secret_key) {
            server_address = "127.0.0.1";
            server_port = 3100;
            heartbeat_timeout = 60 * 5; // Seconds
            quit_counter = 10; // Number of rounds to wait before quitting

            size_t key_length = secret_key.size();
            if (key_length != 32)
            {
                std::cerr << "Secret key must be exactly 32 bytes long, but has " << key_length << " chars.\n";
                std::exit(EXIT_FAILURE);
            }

            // TODO: convert string to array of bytes.
        }

        // Disable all other Special member functions:
        NCConfiguration() = delete;
        NCConfiguration(const NCConfiguration&) = delete;
        NCConfiguration& operator=(const NCConfiguration&) = delete;
        NCConfiguration (NCConfiguration&&) = delete;
        NCConfiguration& operator=(NCConfiguration&&) = delete;
};

#endif // FILE_NC_CONFIG_HPP_INCLUDED