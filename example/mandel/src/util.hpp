/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file includes some utils for the mandel example.
*/

// External includes:

#include <stdfloat>
#include <cstdint>
#include <vector>
#include <memory>

const uint8_t FLOAT_SIZE = 8;
const uint8_t UINT32_SIZE = 4;
const uint32_t MANDEL_DATA_SIZE = (4 * FLOAT_SIZE) + (3 * UINT32_SIZE);

class MandelData {
    public:
        MandelData();
        MandelData(std::vector<uint8_t> data);

        std::vector<uint8_t> to_vector();

        std::float64_t re1, re2, im1, im2;
        uint32_t width, height, max_iteration;
};
