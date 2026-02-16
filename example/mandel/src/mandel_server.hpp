/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file includes the server code for the mandel example.
*/

#ifndef FILE_MANDEL_SERVER_HPP_INCLUDED
#define FILE_MANDEL_SERVER_HPP_INCLUDED

// STD includes:
#include <unordered_map>

// Local includes:
#include "nodcru2/nc_server.hpp"
#include "util.hpp"

using namespace nodcru2;

enum struct JobStatus {
    UnProcessed,
    Processing,
    Done
};

class MandelServerProcessor: public NCServerDataProcessor {
    public:
        [[nodiscard]] std::vector<uint8_t> nc_get_init_data() override;
        [[nodiscard]] bool nc_is_job_done() override;
        void nc_save_data() override;
        void nc_node_timeout(NCNodeID node_id) override;
        [[nodiscard]] std::vector<uint8_t> nc_get_new_data(NCNodeID node_id) override;
        void nc_process_result(NCNodeID node_id, std::vector<uint8_t> result) override;

        MandelServerProcessor(MandelData mandel_data);

        MandelData mandel_data_intern;
        std::vector<JobStatus> mandel_job;
        std::vector<uint32_t> mandel_image;
        std::unordered_map<NCNodeID, uint32_t> node_map;
};

#endif // FILE_MANDEL_SERVER_HPP_INCLUDED
