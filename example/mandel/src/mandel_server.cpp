/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file includes the server code for the mandel example.
*/

// STD include:

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
    spdlog::info("Save the mandel image to disk.");
    // TODO: save as ppm
}

void MandelServerProcessor::nc_node_timeout(NCNodeID node_id) {
    spdlog::debug("Node timeout: {}", node_id);

    if (node_map.contains(node_id)) {
        uint32_t i = node_map[node_id];
        if (mandel_job[i] == JobStatus::Processing) {
            // Give another node a chance to process this job / line:
            mandel_job[i] = JobStatus::UnProcessed;
        } else {
            spdlog::debug("Job at {} not processing.", i);
        }
        node_map.erase(node_id);
    } else {
        spdlog::debug("Timeout node {} not found in map.", node_id);
    }
}

[[nodiscard]] std::vector<uint8_t> MandelServerProcessor::nc_get_new_data(NCNodeID node_id) {
    spdlog::debug("New data for node: {}", node_id);

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
    spdlog::debug("Processed data from node: {}", node_id);

    if (node_map.contains(node_id)) {
        uint32_t i = node_map[node_id];

        if (mandel_job[i] == JobStatus::Processing) {
            mandel_job[i] = JobStatus::Done;
            if (result.size() == mandel_data_intern.width * 4) {
                std::memcpy(&mandel_image[i * mandel_data_intern.height], result.data(), result.size());
            } else {
                spdlog::error("Size missmatch, expected: {}, got: {}", mandel_data_intern.width * 4, result.size());
            }
        } else {
            spdlog::error("Not processing at index {}", i);
        }
    } else {
        spdlog::error("Node not found in node map: {}", node_id);
    }
}
