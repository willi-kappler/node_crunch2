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
#include <filesystem>
#include <string_view>

namespace NodeCrunch2 {
class NCConfiguration {
    public:
        std::string server_address;
        uint16_t server_port;
        uint16_t heartbeat_timeout;
        uint8_t quit_counter;
        std::string secret_key;

        // Constructor:
        NCConfiguration(std::string secret_key_user);

        // Default special member functions:
        NCConfiguration(NCConfiguration&&) = default;
        NCConfiguration(const NCConfiguration&) = default;

        // Disable all other special member functions:
        NCConfiguration() = delete;
        NCConfiguration& operator=(const NCConfiguration&) = delete;
        NCConfiguration& operator=(NCConfiguration&&) = delete;
};

[[nodiscard]] NCConfiguration nc_config_from_string(std::string_view);

[[nodiscard]] NCConfiguration nc_config_from_file(std::filesystem::path);
}

#endif // FILE_NC_CONFIG_HPP_INCLUDED