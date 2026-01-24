/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines the logger functions
*/

#ifndef FILE_NC_LOGGER_HPP_INCLUDED
#define FILE_NC_LOGGER_HPP_INCLUDED

// STD includes:
#include<string>

// External includes:
#include <spdlog/spdlog.h>

namespace NodeCrunch2 {
std::shared_ptr<spdlog::logger> nc_get_logger();

void nc_set_log_level(spdlog::level::level_enum level);

void nc_log_debug(std::string message);

void nc_log_info(std::string message);

void nc_log_error(std::string message);

std::string nc_gen_log_file_name(std::string prefix);

void nc_server_logger(spdlog::level::level_enum log_level = spdlog::level::debug);

void nc_node_logger(spdlog::level::level_enum log_level = spdlog::level::debug);
}

#endif // FILE_NC_LOGGER_HPP_INCLUDED
