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

// External includes:
#include <spdlog/spdlog.h>

namespace NodeCrunch2 {
const size_t NC_NODEID_LENGTH = 64;

class NCNodeID {
    public:
        std::string id;
        NCNodeID();
        bool operator==(const NCNodeID&) const = default;

    private:
        std::string gen_id();
};
}

// From cpp ref: https://en.cppreference.com/w/cpp/utility/hash.html
template<> struct std::hash<NodeCrunch2::NCNodeID> {
    std::size_t operator()(NodeCrunch2::NCNodeID const& node) const noexcept {
        return std::hash<std::string>{}(node.id);
    }
};

// For spdlog:
// Specialization of fmt::formatter for NCNodeID
template <>
struct fmt::formatter<NodeCrunch2::NCNodeID> {
    // Parses format specs like {:x}; we'll just support the default {}:
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        return ctx.begin();
    }

    // Format the real mandel data:
    template <typename FormatContext>
    auto format(const NodeCrunch2::NCNodeID& node, FormatContext& ctx) const -> decltype(ctx.out()) {
        return fmt::format_to(ctx.out(), "[NCNodeID: {}]", node.id);
    }
};

#endif // FILE_NC_NODEID_HPP_INCLUDED