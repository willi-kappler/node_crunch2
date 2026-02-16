/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file includes some utils for the mandel example.
*/

// STD includes:
#include <cstring>
#include <bit>

// External includes:
// #include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/basic_file_sink.h>

// Internal includes:
#include "util.hpp"
#include "nc_logger.hpp"

using namespace nodcru2;

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

std::vector<uint8_t> u32_to_vec_u8(uint32_t in) {
    std::vector<uint8_t> result(4);
    uint8_t* ptr = result.data();

    std::memcpy(ptr, &in, UINT32_SIZE);

    return result;
}

uint32_t vec_u8_to_u32(std::vector<uint8_t> in) {
    uint32_t result;

    std::memcpy(&result, in.data(), UINT32_SIZE);

    return result;
}

void vec_u8_to_vec_u32(std::vector<uint8_t>in, std::vector<uint32_t> out) {
    uint32_t value;

    for (size_t i = 0; i < out.size(); ++i) {
        std::memcpy(&value, &in[i * 4], UINT32_SIZE);

        if (std::endian::native == std::endian::little) {
            value = std::byteswap(value);
        }

        out[i] = value;
    }
}

void vec_u32_to_vec_u8(std::vector<uint32_t>in, std::vector<uint8_t> out) {
    uint32_t value;

    for (size_t i = 0; i < out.size(); ++i) {
        value = in[i];

        if (std::endian::native == std::endian::little) {
            value = std::byteswap(value);
        }

        std::memcpy(&out[i * 4], &value, UINT32_SIZE);
    }
}

void mandel_server_logger(spdlog::level::level_enum log_level) {
    std::shared_ptr<spdlog::logger> file_logger = spdlog::basic_logger_mt("mandel_logger", "mandel_server.log");
    file_logger->set_level(log_level);
}

void mandel_node_logger(spdlog::level::level_enum log_level) {
    std::string file_name = nc_gen_log_file_name("mandel_node");
    std::shared_ptr<spdlog::logger> file_logger = spdlog::basic_logger_mt("mandel_logger", file_name);
    file_logger->set_level(log_level);
}
