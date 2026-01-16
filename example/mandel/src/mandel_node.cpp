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
    mandel_data = MandelData(data);
    spdlog::debug("Initial data received: {}", mandel_data);
}

[[nodiscard]] std::vector<uint8_t> MandelNodeProcessor::nc_process_data(std::vector<uint8_t> data) {
    std::vector<uint8_t> result;

    if (data.size() < UINT32_SIZE) {
        return result;
    }

    uint32_t current_row = 0;
    std::memcpy(&current_row, data.data(), UINT32_SIZE);

    spdlog::debug("Data to process received: {} (current row)", current_row);

    std::vector<uint32_t> line(mandel_data.width);


    result.resize(mandel_data.width * UINT32_SIZE);
    // std::memcpy(dest.data(), source.data(), dest.size());

    return result;
}
