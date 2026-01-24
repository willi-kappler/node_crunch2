/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file includes the server code for the mandel example.
*/

// STD include:
#include <iostream>
#include <fstream>
#include <stdfloat>

// External includes:
#include <spdlog/spdlog.h>

// Internal includes:
#include "mandel_server.hpp"

MandelServerProcessor::MandelServerProcessor(MandelData mandel_data):
    NCServerDataProcessor(),
    mandel_data_intern(mandel_data),
    mandel_job(mandel_data.height),
    mandel_image(mandel_data.width * mandel_data.height),
    node_map()
    {}

[[nodiscard]] std::vector<uint8_t> MandelServerProcessor::nc_get_init_data() {
    return mandel_data_intern.to_vector();
}

[[nodiscard]] bool MandelServerProcessor::nc_is_job_done() {
    for (auto job: mandel_job) {
        if (job != JobStatus::Done) {
            return false;
        }
    }

    return true;
}

void MandelServerProcessor::nc_save_data() {
    spdlog::info("Mandel: Save the mandel image to disk.");

    std::ofstream mandel_file("mandel_image.ppm");
    mandel_file << "P3" << std::endl;
    mandel_file << mandel_data_intern.width << " " << mandel_data_intern.height << std::endl;
    mandel_file << "255" << std::endl;

    std::float64_t pixel_value = 0.0;
    const std::float64_t scale_factor = std::float64_t(mandel_data_intern.max_iteration);

    for (uint32_t i = 0; i < mandel_data_intern.height; i++) {
        for (uint32_t j = 0; j < mandel_data_intern.width; j++) {
            pixel_value = std::float64_t(mandel_image[(mandel_data_intern.height * i) + j]) / scale_factor;

            if (pixel_value < 0.5) {
                mandel_file << "255 " << uint32_t((pixel_value * 2.0) * 255.0) << " 0 ";
            } else {
                mandel_file << uint32_t(((1.0 - pixel_value) * 2.0) * 255.0) << " 0 0 ";
            }
        }
        mandel_file << std::endl;
    }

    mandel_file << std::endl;
    mandel_file.close();
}

void MandelServerProcessor::nc_node_timeout(NCNodeID node_id) {
    spdlog::debug("Mandel: Node timeout: {}", node_id);

    if (node_map.contains(node_id)) {
        uint32_t i = node_map[node_id];
        if (mandel_job[i] == JobStatus::Processing) {
            // Give another node a chance to process this job / line:
            mandel_job[i] = JobStatus::UnProcessed;
        } else {
            spdlog::debug("Mandel: Job at {} not processing.", i);
        }
        node_map.erase(node_id);
    } else {
        spdlog::debug("Mandel: Timeout node {} not found in map.", node_id);
    }
}

[[nodiscard]] std::vector<uint8_t> MandelServerProcessor::nc_get_new_data(NCNodeID node_id) {
    spdlog::debug("Mandel: New data for node: {}", node_id);

    for (uint32_t i = 0; i < mandel_data_intern.height; i++) {
        if (mandel_job[i] == JobStatus::UnProcessed) {
            mandel_job[i] = JobStatus::Processing;
            node_map[node_id] = i;

            return u32_to_vec_u8(i);
        }
    }

    // No more lines to process:
    return {255, 255, 255, 255};
}

void MandelServerProcessor::nc_process_result(NCNodeID node_id, std::vector<uint8_t> result) {
    spdlog::debug("Mandel: Processed data from node: {}", node_id);
    // spdlog::get("nc_logger")->flush();

    if (node_map.contains(node_id)) {
        uint32_t i = node_map[node_id];

        if (mandel_job[i] == JobStatus::Processing) {
            mandel_job[i] = JobStatus::Done;
            if (result.size() == mandel_data_intern.width * 4) {
                std::memcpy(&mandel_image[i * mandel_data_intern.height], result.data(), result.size());
            } else {
                spdlog::error("Mandel: Size missmatch, expected: {}, got: {}", mandel_data_intern.width * 4, result.size());
            }
        } else {
            spdlog::error("Mandel: Not processing at index {}", i);
        }
    } else {
        spdlog::error("Mandel: Node not found in node map: {}", node_id);
    }
}
