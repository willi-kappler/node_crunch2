/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines the node class
*/

// STD includes:
#include <thread>
#include <chrono>

// Local includes:
#include "nc_node.hpp"

NCNode::NCNode(NCConfiguration config):
    server_address(config.server_address),
    server_port(config.server_port),
    heartbeat_timeout(config.heartbeat_timeout),
    secret_key(config.secret_key),
    node_id(NCNodeID()),
    quit(false) {
}

NCNode::~NCNode(){
}

void NCNode::nc_run() {
}

void NCNode::nc_send_msg_return_answer(std::vector<uint8_t>) {

}

void NCNode::nc_send_heartbeat() {
    auto sleep_time = std::chrono::seconds(heartbeat_timeout);

    while (!quit.load()) {
        std::this_thread::sleep_for(sleep_time);
    }
}
