/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines the configuration options
*/

// STD includes:
#include <string_view>

// External includes:
#include <tao/json.hpp>

// Local includes:
#include "nc_config.hpp"

NCConfiguration nc_config_from_file(std::string_view filename) {
    const tao::json::value json_config = tao::json::from_file(filename);

    // Must always be present:
    std::string secret_key = json_config.at("secret_key").as<std::string>();
    NCConfiguration config = NCConfiguration(secret_key);

    if (auto v = json_config.find("server_address"); v != nullptr) {
        config.server_address = v->as<std::string>();
    }

    if (auto v = json_config.find("server_port"); v != nullptr) {
        config.server_port = v->as<uint16_t>();
    }

    if (auto v = json_config.find("heartbeat_timeout"); v != nullptr) {
        config.heartbeat_timeout = v->as<uint16_t>();
    }

    if (auto v = json_config.find("quit_counter"); v != nullptr) {
        config.quit_counter = v->as<uint8_t>();
    }

    return config;
}
