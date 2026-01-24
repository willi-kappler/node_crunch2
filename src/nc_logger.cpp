/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines the logger functions
*/

// STD includes:
#include <filesystem>

// External includes:
#include <spdlog/sinks/null_sink.h>
#include <spdlog/sinks/basic_file_sink.h>

// Local includes:
#include "nc_logger.hpp"

namespace NodeCrunch2 {
std::shared_ptr<spdlog::logger> nc_get_logger() {
    auto logger = spdlog::get("nc_logger");

    if (!logger) {
        logger = spdlog::null_logger_mt("nc_logger");
    }

    return logger;
}

void nc_set_log_level(spdlog::level::level_enum level) {
    nc_get_logger()->set_level(level);
}

void nc_log_debug(std::string message) {
    nc_get_logger()->debug(message);
}

void nc_log_info(std::string message) {
    nc_get_logger()->info(message);
}

void nc_log_error(std::string message) {
    nc_get_logger()->error(message);
}

std::string nc_gen_log_file_name(std::string prefix) {
    uint32_t file_counter = 1;
    std::string file_name = fmt::format("{}_001.log", prefix);

    while (true) {
        if (std::filesystem::exists(file_name)) {
            file_counter++;
            file_name = fmt::format("{}_{:03}.log", prefix, file_counter);
        } else {
            break;
        }
    }

    return file_name;
}

void nc_server_logger(spdlog::level::level_enum log_level) {
    spdlog::drop("nc_logger");
    std::shared_ptr<spdlog::logger> file_logger = spdlog::basic_logger_mt("nc_logger", "nc_server.log");
    file_logger->set_level(log_level);
}

void nc_node_logger(spdlog::level::level_enum log_level) {
    spdlog::drop("nc_logger");
    std::string file_name = nc_gen_log_file_name("nc_node");
    std::shared_ptr<spdlog::logger> file_logger = spdlog::basic_logger_mt("nc_logger", file_name);
    file_logger->set_level(log_level);
}
}
