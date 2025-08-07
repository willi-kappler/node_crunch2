/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines some helper functions.
*/

// Local includes:
#include "nc_util.hpp"

void nc_to_big_endian_bytes(uint32_t const value, std::span<uint8_t> bytes) noexcept {
    uint32_t final_value = value;

    if (std::endian::native == std::endian::little) {
        final_value = std::byteswap(value);
    }

    std::memcpy(bytes.data(), &final_value, sizeof(uint32_t));
}

uint32_t nc_from_big_endian_bytes(std::span<const uint8_t> const bytes) {
    uint32_t result;

    std::memcpy(&result, bytes.data(), sizeof(uint32_t));

    if (std::endian::native == std::endian::little) {
        result = std::byteswap(result);
    }

    return result;
}

[[nodiscard]] std::string nc_type_to_string(NCMessageType const& msg_type) {
    std::string result = "Unknown type";

    switch (msg_type) {
        case NCMessageType::Unknown:
            result = "Unknown";
        break;
        case NCMessageType::Heartbeat:
            result = "Heartbeat";
        break;
        case NCMessageType::HeartbeatOK:
            result = "HeartbeatOK";
        break;
        case NCMessageType::HeartbeatError:
            result = "HeartbeatError";
        break;
        case NCMessageType::Init:
            result = "Init";
        break;
        case NCMessageType::InitOK:
            result = "InitOK";
        break;
        case NCMessageType::InitError:
            result = "InitError";
        break;
        case NCMessageType::NewDataFromServer:
            result = "NewDataFromServer";
        break;
        case NCMessageType::NewResultFromNode:
            result = "NewResultFromNode";
        break;
        case NCMessageType::NodeNeedsMoreData:
            result = "NodeNeedsMoreData";
        break;
        case NCMessageType::ResultOK:
            result = "ResultOK";
        break;
        case NCMessageType::ConnectionError:
            result = "ConnectionError";
        break;
        case NCMessageType::Quit:
            result = "Quit";
        break;
    }

    return result;
}

[[nodiscard]] std::string nc_error_to_str(NCMessageError const& msg_error) {
    std::string result = "Unknown error";

    switch (msg_error) {
        case NCMessageError::CipherContextError:
            result = "";
        break;
        case NCMessageError::EncryptInitError:
            result = "";
        break;
        case NCMessageError::DecryptInitError:
            result = "";
        break;
        case NCMessageError::CipherControllError:
            result = "";
        break;
        case NCMessageError::CreateNonceError:
            result = "";
        break;
        case NCMessageError::SetNonceError:
            result = "";
        break;
        case NCMessageError::EncryptUpdateError:
            result = "";
        break;
        case NCMessageError::DecryptUpdateError:
            result = "";
        break;
        case NCMessageError::EncryptFinalError:
            result = "";
        break;
        case NCMessageError::DecryptFinalError:
            result = "";
        break;
        case NCMessageError::CipherGetTagError:
            result = "";
        break;
        case NCMessageError::CipherSetTagError:
            result = "";
        break;
        case NCMessageError::CompressionError:
            result = "";
        break;
        case NCMessageError::DecompressionError:
            result = "";
        break;
        case NCMessageError::EncryptionError:
            result = "";
        break;
        case NCMessageError::DecryptionError:
            result = "";
        break;
        case NCMessageError::SizeMissmatch:
            result = "";
        break;
        case NCMessageError::NetworkConnectError:
            result = "";
        break;
        case NCMessageError::NetworkWriteError:
            result = "";
        break;
        case NCMessageError::NetworkReadError:
            result = "";
        break;
    }

    return result;
}
