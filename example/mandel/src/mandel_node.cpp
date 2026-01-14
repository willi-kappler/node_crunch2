/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file includes the node code for the mandel example.
*/

#include "mandel_node.hpp"

MandelNodeProcessor::MandelNodeProcessor():
    NCNodeDataProcessor(),
    mandel_data()
    {}

void MandelNodeProcessor::nc_init(std::vector<uint8_t> data) {
    initial_data = data;
}

[[nodiscard]] std::vector<uint8_t> MandelNodeProcessor::nc_process_data(std::vector<uint8_t> data) {
    std::vector<uint8_t> result;

    return result;
}
