/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines the configuration options
*/

// STD includes:
#include <fstream>

// External includes:
#include <tao/json.hpp>

// Local includes:
#include "nc_config.hpp"


std::expected<NCConfiguration, NCConfigurationError> nc_config_from_string(std::string_view config_as_string) {
    const tao::json::value json_config = tao::json::from_string(config_as_string);

    // Secret key must always be present.
    if (auto v = json_config.find("secret_key"); v == nullptr) {
        return std::unexpected(NCConfigurationError::NCMissingSecretKey);
    }

    std::string secret_key = json_config.at("secret_key").as<std::string>();
    NCConfiguration config = NCConfiguration(secret_key);

    if (auto v = json_config.find("server_address"); v != nullptr) {
        config.server_address = v->as<std::string>();
    }

    if (auto v = json_config.find("server_port"); v != nullptr) {
        config.server_port = v->as<uint16_t>();

        if (config.server_port < 1) {
            return std::unexpected(NCConfigurationError::NCInvalidPort);
        }
    }

    if (auto v = json_config.find("heartbeat_timeout"); v != nullptr) {
        config.heartbeat_timeout = v->as<uint16_t>();

        if (config.heartbeat_timeout < 10) {
            return std::unexpected(NCConfigurationError::NCInvalidHeartbeat);
        }
    }

    if (auto v = json_config.find("quit_counter"); v != nullptr) {
        config.quit_counter = v->as<uint8_t>();
    }

    return config;
}

std::expected<NCConfiguration, NCConfigurationError> nc_config_from_file(std::filesystem::path file_path) {
    std::ifstream in_file {file_path};

    if (in_file.is_open()) {
        std::string file_contents {std::istreambuf_iterator<char>(in_file), std::istreambuf_iterator<char>()};
        return nc_config_from_string(file_contents);
    } else {
        return std::unexpected(NCConfigurationError::NCFileOpenError);
    }
}
