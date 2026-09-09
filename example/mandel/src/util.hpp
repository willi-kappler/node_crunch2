/*
    Node Crunch2
    SPDX-License-Identifier: MIT
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file includes some utils for the mandel example.
*/

#ifndef FILE_MANDEL_UTIL_HPP_INCLUDED
#define FILE_MANDEL_UTIL_HPP_INCLUDED

// STD includes:
#include <stdfloat>
#include <cstdint>
#include <vector>
#include <memory>

// Size in bytes:
const uint8_t FLOAT_SIZE = 8;
const uint8_t UINT32_SIZE = 4;
const uint32_t MANDEL_DATA_SIZE = (4 * FLOAT_SIZE) + (3 * UINT32_SIZE);

class MandelData {
    public:
        MandelData();
        MandelData(std::vector<uint8_t> data);

        [[nodiscard]] std::vector<uint8_t> to_vector();
        [[nodiscard]] std::string to_string();

        std::float64_t re1, re2, im1, im2, re_step, im_step;
        uint32_t width, height, max_iteration;
};

[[nodiscard]] std::vector<uint8_t> u32_to_vec_u8(uint32_t in);

[[nodiscard]] uint32_t vec_u8_to_u32(std::vector<uint8_t> in);

void vec_u8_to_vec_u32(std::vector<uint8_t> vec_in, std::vector<uint32_t> vec_out);

void vec_u32_to_vec_u8(std::vector<uint32_t> vec_in, std::vector<uint8_t> vec_out);

void mandel_server_logger(spdlog::level::level_enum log_level = spdlog::level::debug);

void mandel_node_logger(spdlog::level::level_enum log_level = spdlog::level::debug);

#endif // FILE_MANDEL_UTIL_HPP_INCLUDED
