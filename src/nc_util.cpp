/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines some helper functions.
*/

// Local includes:
#include "nc_util.hpp"

namespace NodeCrunch2 {
void nc_to_big_endian_bytes(uint32_t const value, std::span<uint8_t> bytes) noexcept {
    uint32_t final_value = value;

    if (std::endian::native == std::endian::little) {
        final_value = std::byteswap(value);
    }

    std::memcpy(bytes.data(), &final_value, sizeof(uint32_t));
}

[[nodiscard]] uint32_t nc_from_big_endian_bytes(std::span<const uint8_t> const bytes) {
    uint32_t result;

    std::memcpy(&result, bytes.data(), sizeof(uint32_t));

    if (std::endian::native == std::endian::little) {
        result = std::byteswap(result);
    }

    return result;
}

[[nodiscard]] std::string nc_type_to_string(NCNodeMessageType const& msg_type) {
    std::string result = "Unknown type";

    switch (msg_type) {
        case NCNodeMessageType::Init:
            result = "Init";
        break;
        case NCNodeMessageType::Heartbeat:
            result = "Heartbeat";
        break;
        case NCNodeMessageType::NewResultFromNode:
            result = "NewResultFromNode";
        break;
        case NCNodeMessageType::NodeNeedsMoreData:
            result = "NodeNeedsMoreData";
        break;
    }

    return result;
}

[[nodiscard]] std::string nc_type_to_string(NCServerMessageType const& msg_type) {
    std::string result = "Unknown type";

    switch (msg_type) {
        case NCServerMessageType::UnknownError:
            result = "UnknownError";
        break;
        case NCServerMessageType::HeartbeatOK:
            result = "HeartbeatOK";
        break;
        case NCServerMessageType::InitOK:
            result = "InitOK";
        break;
        case NCServerMessageType::NewDataFromServer:
            result = "NewDataFromServer";
        break;
        case NCServerMessageType::ResultOK:
            result = "ResultOK";
        break;
        case NCServerMessageType::InvalidNodeID:
            result = "InvalidNodeID";
        break;
        case NCServerMessageType::Quit:
            result = "Quit";
        break;
    }

    return result;
}

}
