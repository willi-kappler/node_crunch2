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
}

[[nodiscard]] std::vector<uint8_t> MandelServerProcessor::nc_get_new_data(NCNodeID node_id) {
    spdlog::debug("New data for node: {}", node_id);

}

void MandelServerProcessor::nc_process_result(NCNodeID node_id, std::vector<uint8_t> result) {
    spdlog::debug("Processed data from node: {}", node_id);

}
