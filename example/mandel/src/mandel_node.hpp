/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file includes the node code for the mandel example.
*/

#ifndef MANDEL_NODE_HPP_INCLUDED
#define MANDEL_NODE_HPP_INCLUDED

// STD includes:
#include <chrono>

// Local includes:
#include "nc_node.hpp"
#include "util.hpp"

using namespace NodeCrunch2;

class MandelNodeProcessor: public NCNodeDataProcessor {
    public:
        void nc_init(std::vector<uint8_t> data) override;
        [[nodiscard]] std::vector<uint8_t> nc_process_data(std::vector<uint8_t> data) override;

        MandelNodeProcessor();

    private:
        MandelData mandel_data;
        std::chrono::seconds compute_time;
};

#endif // MANDEL_NODE__HPP_INCLUDED
