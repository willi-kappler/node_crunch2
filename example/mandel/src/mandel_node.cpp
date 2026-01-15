/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file includes the node code for the mandel example.
*/

// External includes:
#include <spdlog/spdlog.h>

// Internal includes:
#include "mandel_node.hpp"

MandelNodeProcessor::MandelNodeProcessor():
    NCNodeDataProcessor(),
    mandel_data()
    {}

void MandelNodeProcessor::nc_init(std::vector<uint8_t> data) {
    spdlog::debug("Initial data received: {}", data[0]);
}

[[nodiscard]] std::vector<uint8_t> MandelNodeProcessor::nc_process_data(std::vector<uint8_t> data) {
    spdlog::debug("Data to process received: {}", data[0]);

    std::vector<uint8_t> result;

    return result;
}
