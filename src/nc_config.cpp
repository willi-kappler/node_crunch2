/*
    Node Crunch2
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
    secret_key(secret_key_user)
{
    size_t key_length = secret_key_user.size();
    if (key_length != 32)
    {
        std::cerr << "Secret key must be exactly 32 bytes long, but has " << key_length << " chars.\n";
        throw NCInvalidKeyException();
    }
}

[[nodiscard]] NCConfiguration nc_config_from_json(const tao::json::value json_config) {
    // Secret key must always be present.
    if (auto v = json_config.find("secret_key"); v == nullptr) {
        throw NCConfigurationException("Missing secret key");
    }

    std::string secret_key = json_config.at("secret_key").as<std::string>();
    NCConfiguration config = NCConfiguration(secret_key);

    if (auto v = json_config.find("server_address"); v != nullptr) {
        config.server_address = v->as<std::string>();
    }

    if (auto v = json_config.find("server_port"); v != nullptr) {
        config.server_port = v->as<uint16_t>();

        if (config.server_port < 1) {
            throw NCConfigurationException("Invalid port");
        }
    }

    if (auto v = json_config.find("heartbeat_timeout"); v != nullptr) {
        config.heartbeat_timeout = v->as<uint16_t>();

        if (config.heartbeat_timeout < 10) {
            throw NCConfigurationException("Invalid heartbeat");
        }
    }

    if (auto v = json_config.find("quit_counter"); v != nullptr) {
        config.quit_counter = v->as<uint8_t>();
    }

    return config;
}

[[nodiscard]] NCConfiguration nc_config_from_string(std::string_view config_as_string) {
    const tao::json::value json_config = tao::json::from_string(config_as_string);
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
