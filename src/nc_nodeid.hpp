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

#ifndef FILE_NC_NODEID_HPP_INCLUDED
#define FILE_NC_NODEID_HPP_INCLUDED

// STD includes:
#include <string>
#include <random>

const size_t NC_ID_LENGTH = 64;
const std::string NC_CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
static std::random_device nc_rd;
static std::mt19937 nc_gen(nc_rd());
static std::uniform_int_distribution<> nc_dis(0, static_cast<int>(NC_CHARACTERS.size()) - 1);

class NCNodeID {
    public:
        std::string id;

        NCNodeID(): id(gen_id()) {}

        bool operator==(const NCNodeID&) const = default;

    private:
        std::string gen_id() {
            std::string result;

            for (size_t i = 0; i < NC_ID_LENGTH; i++) {
                result += NC_CHARACTERS[nc_dis(nc_gen)];
            }

            return result;
        }
};

// From cpp ref: https://en.cppreference.com/w/cpp/utility/hash.html
template<> struct std::hash<NCNodeID> {
    std::size_t operator()(NCNodeID const& node) const noexcept {
        return std::hash<std::string>{}(node.id);
    }
};

#endif // FILE_NC_NODEID_HPP_INCLUDED