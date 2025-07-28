/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines the node class
*/

#include "nc_node.hpp"

NCNode::NCNode(NCConfiguration config):
    server_address(config.server_address),
    server_port(config.server_port),
    heartbeat_timeout(config.heartbeat_timeout),
    secret_key(config.secret_key),
    node_id(NCNodeID()) {
}

NCNode::~NCNode(){
}

void NCNode::nc_run() {
}

void NCNode::nc_init(std::vector<uint8_t> data) {
    if (data.size() > 0) {

    }
}

std::vector<uint8_t> NCNode::nc_process_data(std::vector<uint8_t>) {
    std::vector<uint8_t> result;

    return result;
}
