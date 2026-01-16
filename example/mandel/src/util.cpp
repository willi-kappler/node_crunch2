/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file includes some utils for the mandel example.
*/

// External includes:
#include <cstring>
#include <spdlog/spdlog.h>

// Internal includes:
#include "util.hpp"

MandelData::MandelData():
    re1(-2.0),
    re2(1.0),
    im1(-1.5),
    im2(1.5),
    re_step(0.0),
    im_step(0.0),
    width(2048),
    height(2048),
    max_iteration(2048) {
        re_step = ((re2 - re1) / std::float64_t(width));
        im_step = ((im2 - im1) / std::float64_t(height));
    }

MandelData::MandelData(std::vector<uint8_t> data):
    // Delegating constructor
    MandelData() {
    if (data.size() < MANDEL_DATA_SIZE) {
        // Basic safety check
        return;
    }

    const uint8_t* ptr = data.data();

    // Get the actual values from the data block:
    // All floats:
    std::memcpy(&re1, ptr, FLOAT_SIZE);
    ptr += FLOAT_SIZE;
    std::memcpy(&re2, ptr, FLOAT_SIZE);
    ptr += FLOAT_SIZE;
    std::memcpy(&im1, ptr, FLOAT_SIZE);
    ptr += FLOAT_SIZE;
    std::memcpy(&im2, ptr, FLOAT_SIZE);
    ptr += FLOAT_SIZE;

    // All ints:
    std::memcpy(&width, ptr, UINT32_SIZE);
    ptr += UINT32_SIZE;
    std::memcpy(&height, ptr, UINT32_SIZE);
    ptr += UINT32_SIZE;
    std::memcpy(&max_iteration, ptr, UINT32_SIZE);

    re_step = ((re2 - re1) / std::float64_t(width));
    im_step = ((im2 - im1) / std::float64_t(height));
}

std::vector<uint8_t> MandelData::to_vector() {
    std::vector<uint8_t> result;
    result.resize(MANDEL_DATA_SIZE);
    uint8_t* ptr = result.data();

    // Write all the values to the data block:
    // All floats:
    std::memcpy(ptr, &re1, FLOAT_SIZE);
    ptr += FLOAT_SIZE;
    std::memcpy(ptr, &re2, FLOAT_SIZE);
    ptr += FLOAT_SIZE;
    std::memcpy(ptr, &im1, FLOAT_SIZE);
    ptr += FLOAT_SIZE;
    std::memcpy(ptr, &im2, FLOAT_SIZE);
    ptr += FLOAT_SIZE;

    // All ints:
    std::memcpy(ptr, &width, UINT32_SIZE);
    ptr += UINT32_SIZE;
    std::memcpy(ptr, &height, UINT32_SIZE);
    ptr += UINT32_SIZE;
    std::memcpy(ptr, &max_iteration, UINT32_SIZE);

    return result;
}
