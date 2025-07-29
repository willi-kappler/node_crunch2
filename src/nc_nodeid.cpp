/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines the unique node ID that is assigned to each node.
    When a new node registers for the first time to the server the node's own
    node ID is send to the server. Each message from the node to the server
    contains this unique node id. If the node ID is unknown to the server,
    it sends an error to the node.
*/
// STD includes:
#include <random>

// Local includes:
#include "nc_nodeid.hpp"

const std::string NC_CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
std::random_device nc_rd;
std::mt19937 nc_gen(nc_rd());
std::uniform_int_distribution nc_dis(0, static_cast<int>(NC_CHARACTERS.size()) - 1);

NCNodeID::NCNodeID(): id(gen_id()) {}

std::string NCNodeID::gen_id() {
    std::string result;

    for (size_t i = 0; i < NC_NODEID_LENGTH; i++) {
        result += NC_CHARACTERS[nc_dis(nc_gen)];
    }

    return result;
}
