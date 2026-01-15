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

class MandelData {
    public:
        MandelData();
        MandelData(std::vector<uint8_t> data);

        std::float64_t re1, re2, im1, im2;
        uint32_t width, height;
};
