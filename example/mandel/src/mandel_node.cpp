/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file includes the node code for the mandel example.
*/

// STD include:
#include <chrono>

// External includes:
#include <spdlog/spdlog.h>

// Internal includes:
#include "mandel_node.hpp"

MandelNodeProcessor::MandelNodeProcessor():
    NCNodeDataProcessor(),
    mandel_data(),
    compute_time(1)
    {}

void MandelNodeProcessor::nc_init(std::vector<uint8_t> data) {
    mandel_data = MandelData(data);
    spdlog::debug("Initial data received: {}", mandel_data);
}

[[nodiscard]] std::vector<uint8_t> MandelNodeProcessor::nc_process_data(std::vector<uint8_t> data) {
    std::vector<uint8_t> result;

    if (data.size() != UINT32_SIZE) {
        // Size does not match, we expect 4 bytes.
        return result;
    }

    uint32_t i;
    uint32_t current_iter;
    uint32_t current_row = 0;
    std::memcpy(&current_row, data.data(), UINT32_SIZE);

    if (current_row >= mandel_data.height) {
        // Out of bounds, larger than image height:
        return result;
    }

    spdlog::debug("Data to process received: {} (current row)", current_row);

    std::vector<uint32_t> line(mandel_data.width);
    std::float64_t im_start = mandel_data.im1 + (std::float64_t(current_row) * mandel_data.im_step);
    std::float64_t z_re, z_im, z2_re, z2_im, c_re, c_im;

    for (i = 0; i < mandel_data.width; i++) {
        current_iter = 0;
        c_re = mandel_data.re1 + (std::float64_t(i) * mandel_data.re_step);
        c_im = im_start;
        z_re = c_re;
        z_im = c_im;

        while (current_iter < mandel_data.max_iteration) {
            z2_re = z_re * z_re;
            z2_im = z_im * z_im;

            if (z2_re + z2_im > 4.0) {
                break;
            }

            z_im = (2.0 * z_re * z_im) + c_im;
            z_re = z2_re - z2_im + c_re;

            current_iter++;
        }
        line[i] = current_iter;
    }

    result.resize(mandel_data.width * UINT32_SIZE);
    std::memcpy(result.data(), line.data(), result.size());

    // Simulate some heavy computations:
    std::this_thread::sleep_for(compute_time);

    return result;
}
