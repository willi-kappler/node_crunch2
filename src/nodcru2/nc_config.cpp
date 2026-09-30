/*
    Node Crunch2
    SPDX-License-Identifier: MIT
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines the configuration options
*/

// STD includes:
#include <fstream>
#include <iostream>

// Local includes:
#include "nc_config.hpp"
#include "nc_exceptions.hpp"

namespace nodcru2 {
NCConfiguration::NCConfiguration(std::string secret_key_user):
    // Member initialization list:
    server_address("127.0.0.1"),
    server_port(3100),
    heartbeat_timeout(60 * 5), // Seconds
    quit_counter(10), // Number of rounds to wait before quitting
    secret_key(secret_key_user),
    nc_server_log_file(""),
    nc_server_log_level(""),
    nc_node_log_file(""),
    nc_node_log_level("")
{
    size_t key_length = secret_key_user.size();
    if (key_length != 32)
    {
        std::cerr << "Secret key must be exactly 32 bytes long, but has " << key_length << " chars.\n";
        throw NCInvalidKeyException();
    }
}

[[nodiscard]] NCConfiguration nc_config_from_json(const nlohmann::json json_config) {
    // Secret key must always be present.
    if (!json_config.contains("secret_key")) {
        throw NCConfigurationException("Missing secret key");
    }

    std::string secret_key = json_config["secret_key"].get<std::string>();
    NCConfiguration config = NCConfiguration(secret_key);

    if (json_config.contains("server_address")) {
        config.server_address = json_config["server_address"].get<std::string>();
    }

    if (json_config.contains("server_port")) {
        config.server_port = json_config["server_port"].get<uint16_t>();

        if (config.server_port < 1) {
            throw NCConfigurationException("Invalid port");
        }
    }

    if (json_config.contains("heartbeat_timeout")) {
        config.heartbeat_timeout = json_config["heartbeat_timeout"].get<uint16_t>();

        if (config.heartbeat_timeout < 10) {
            throw NCConfigurationException("Invalid heartbeat");
        }
    }

    if (json_config.contains("quit_counter")) {
        config.quit_counter = json_config["quit_counter"].get<uint8_t>();
    }

    if (json_config.contains("nc_server_log_file")) {
        config.nc_server_log_file = json_config["nc_server_log_file"].get<std::string>();
    }

    if (json_config.contains("nc_server_log_level")) {
        config.nc_server_log_level = json_config["nc_server_log_level"].get<std::string>();
    }

    if (json_config.contains("nc_node_log_file")) {
        config.nc_node_log_file = json_config["nc_node_log_file"].get<std::string>();
    }

    if (json_config.contains("nc_node_log_level")) {
        config.nc_node_log_level = json_config["nc_node_log_level"].get<std::string>();
    }

    return config;
}

[[nodiscard]] NCConfiguration nc_config_from_string(std::string_view config_as_string) {
    const nlohmann::json json_config = nlohmann::json::parse(config_as_string);
    return nc_config_from_json(json_config);
}

[[nodiscard]] NCConfiguration nc_config_from_file(std::filesystem::path file_path) {
    std::ifstream in_file {file_path};

    if (in_file.is_open()) {
        std::string file_contents {std::istreambuf_iterator<char>(in_file), std::istreambuf_iterator<char>()};
        return nc_config_from_string(file_contents);
    } else {
        throw NCConfigurationException("Open file error");
    }
}
}
