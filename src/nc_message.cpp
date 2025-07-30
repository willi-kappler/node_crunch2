/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines the message type and decoding / encoding
*/

// STD includes:
#include <cstring>
//#include <iostream>

// Local includes:
#include "nc_compression.hpp"
#include "nc_encryption.hpp"
#include "nc_message.hpp"

[[nodiscard]] NCDecompressedMessage nc_encode1(uint32_t expected_size, NCMessageType const msg_type, std::vector<uint8_t> const& data) {
    NCDecompressedMessage decompressed_message;
    decompressed_message.data = std::vector<uint8_t>(expected_size);
    uint32_t data_index = 0;

    // Encode message type (1 byte)
    decompressed_message.data[data_index++] = static_cast<uint8_t>(msg_type);

    // Encode the actual data:
    for (uint8_t const v: data) {
        decompressed_message.data[data_index++] = v;
    }

    return decompressed_message;
}

[[nodiscard]] std::expected<std::vector<uint8_t>, NCMessageError> nc_encode2(NCDecompressedMessage const& decompressed_message, std::string const& secret_key) {
    // 2. Compress message:
    auto compressed_message = nc_compress_message(decompressed_message);
    if (!compressed_message) {
        return std::unexpected(compressed_message.error());
    }

    // 3. Encrypt compressed message:
    auto encrypted_message = nc_encrypt_message(NCDecryptedMessage{compressed_message->data}, secret_key);
    if (!encrypted_message) {
        return std::unexpected(encrypted_message.error());
    }

    // 4. Encode encrypted compressed message:
    std::vector<uint8_t> result;
    uint32_t expected_size = static_cast<uint32_t>(NC_NONCE_LENGTH + NC_GCM_TAG_LENGTH + encrypted_message->data.size());

    for (uint8_t const v: encrypted_message->nonce) {
        result.push_back(v);
    }
    for (uint8_t const v: encrypted_message->tag) {
        result.push_back(v);
    }
    for (uint8_t const v: encrypted_message->data) {
        result.push_back(v);
    }

    if (result.size() != expected_size) {
        return std::unexpected(NCMessageError::SizeMissmatch);
    }

    return result;
}

[[nodiscard]] std::expected<NCDecompressedMessage, NCMessageError> nc_decode1(std::vector<uint8_t> const& message, std::string const& secret_key) {
    // 1. Decrypt message:
    NCEncryptedMessage encrypted_message;
    uint32_t source_index = 0;
    uint32_t source_end = static_cast<uint32_t>(message.size());

    // Get nonce:
    for (uint8_t &v: encrypted_message.nonce) {
        v = message[source_index++];
    }

    // Get tag:
    for (uint8_t &v: encrypted_message.tag) {
        v = message[source_index++];
    }

    // Get the rest of the data, if any:
    if (source_index < source_end) {
        encrypted_message.data = std::vector<uint8_t>();
        encrypted_message.data.reserve(source_end - source_index);
        while (source_index < source_end) {
            encrypted_message.data.push_back(message[source_index++]);
        }
    }

    if (source_index != source_end) {
        return std::unexpected(NCMessageError::SizeMissmatch);
    }

    auto decrypted_message = nc_decrypt_message(encrypted_message, secret_key);
    if (!decrypted_message) {
        return std::unexpected(decrypted_message.error());
    }

    // 2. Decompress message:
    return nc_decompress_message(NCCompressedMessage{decrypted_message->data});
}

[[nodiscard]] NCExpEncToServer nc_encode_message_to_server(NCMessageType const msg_type, NCNodeID const& node_id, std::vector<uint8_t> const& data, std::string const& secret_key) {
    // 1. Encode message:
    uint32_t expected_size = static_cast<uint32_t>(2 + data.size() + node_id.id.size());
    NCDecompressedMessage decompressed_message = nc_encode1(expected_size, msg_type, data);
    uint32_t data_index = static_cast<uint32_t>(data.size()) + 1;
    // Encode node id:
    for (uint8_t const v: node_id.id) {
        decompressed_message.data[data_index++] = v;
    }

    std::expected<std::vector<uint8_t>, NCMessageError> result = nc_encode2(decompressed_message, secret_key);

    if (!result) {
        return std::unexpected(result.error());
    } else {
        return NCEncodedMessageToServer{*result};
    }
}

[[nodiscard]] NCExpEncToNode nc_encode_message_to_node(NCMessageType const msg_type, std::vector<uint8_t> const& data, std::string const& secret_key) {
    // 1. Encode message:
    uint32_t expected_size = static_cast<uint32_t>(2 + data.size());
    NCDecompressedMessage decompressed_message = nc_encode1(expected_size, msg_type, data);

    std::expected<std::vector<uint8_t>, NCMessageError> result = nc_encode2(decompressed_message, secret_key);

    if (!result) {
        return std::unexpected(result.error());
    } else {
        return NCEncodedMessageToNode{*result};
    }
}

[[nodiscard]] NCExpDecFromServer nc_decode_message_from_server(NCExpEncToNode const& message, std::string const& secret_key) {
    std::expected<NCDecompressedMessage, NCMessageError> decompressed_message = nc_decode1(message->data, secret_key);

    if (!decompressed_message) {
        return std::unexpected(decompressed_message.error());
    }

    // 3. Decode message:
    NCDecodedMessageFromServer result;
    // Decode message type:
    result.msg_type = static_cast<NCMessageType>(decompressed_message->data[0]);
    uint32_t source_index = 1;
    uint32_t source_end = static_cast<uint32_t>(decompressed_message->data.size());

    // Decode the actual data, if any:
    if (source_index < source_end) {
        result.data = std::vector<uint8_t>();
        result.data.reserve(source_end - source_index);
        while (source_index < source_end) {
            result.data.push_back(decompressed_message->data[source_index++]);
        }
    }

    return result;
}

[[nodiscard]] NCExpDecFromNode nc_decode_message_from_node(NCExpEncToServer const& message, std::string const& secret_key) {
    std::expected<NCDecompressedMessage, NCMessageError> decompressed_message = nc_decode1(message->data, secret_key);

    if (!decompressed_message) {
        return std::unexpected(decompressed_message.error());
    }

    // 3. Decode message:
    NCDecodedMessageFromNode result;
    // Decode message type:
    result.msg_type = static_cast<NCMessageType>(decompressed_message->data[0]);
    uint32_t source_index = 1;
    uint32_t source_end = static_cast<uint32_t>(decompressed_message->data.size()) - NC_NONCE_LENGTH;

    // Decode the actual data, if any:
    if (source_index < source_end) {
        result.data = std::vector<uint8_t>();
        result.data.reserve(source_end - source_index);
        while (source_index < source_end) {
            result.data.push_back(decompressed_message->data[source_index++]);
        }
    }

    // Decode node id:
    for (auto &v: result.node_id.id) {
        v = decompressed_message->data[source_index++];
    }

    return result;
}

[[nodiscard]] NCExpEncToServer nc_gen_heartbeat_message(NCNodeID const& node_id, std::string const& secret_key) {
    /*
    Generate a heartbeat message to be sent from the node to the server.

    The node sends its node_id that the server will check.
    The secret key is used to encode the message.
    */

    return nc_encode_message_to_server(NCMessageType::Heartbeat, node_id, {}, secret_key);
}

[[nodiscard]] NCExpEncToNode nc_gen_heartbeat_message_ok(std::string const& secret_key) {
    /*
    Generate a "heartbeat OK" message to be sent from the server to the node.

    This message is only sent if the heartbeat message from the node was
    sent in the time limit and contained a valid node id.
    The secret key is used to encode the message.
    */

    return nc_encode_message_to_node(NCMessageType::HeartbeatOK, {}, secret_key);
}

[[nodiscard]] NCExpEncToNode nc_gen_heartbeat_message_error(std::string const& secret_key) {
    /*
    Generate a "heartbeat error" message to be sent from the server to the node.

    This message is only sent if the heartbeat message from the node was
    sent too late or did not contain a valid node id.
    The secret key is used to encode the message.
    */

    return nc_encode_message_to_node(NCMessageType::HeartbeatError, {}, secret_key);
}

[[nodiscard]] NCExpEncToServer nc_gen_init_message(NCNodeID const& node_id, std::string const& secret_key) {
    /*
    Generate an initialisation message to be sent from the node to the server.

    This message is only sent once when the node connects for the first time to the server.
    The node registers itself to the server given its own node id.
    The secret key is used to encode the message.
    */

    return nc_encode_message_to_server(NCMessageType::Init, node_id, {}, secret_key);
}

[[nodiscard]] NCExpEncToNode nc_gen_init_message_ok(std::vector<uint8_t> const& init_data, std::string const& secret_key) {
    /*
    Generate an "init ok" message to be sent from the server to the node.

    This message is only sent once when the node has registered itself correctly to the server.
    The server then can send some initial data to the node, if needed.
    The secret key is used to encode the message.
    */

    return nc_encode_message_to_node(NCMessageType::InitOK, init_data, secret_key);
}

[[nodiscard]] NCExpEncToNode nc_gen_init_message_error(std::string const& secret_key) {
    /*
    Generate an "init error" message to be sent from the server to the node.

    This message is only sent when the registration of the new node has failed.
    The secret key is used to encode the message.
    */

    return nc_encode_message_to_node(NCMessageType::InitError, {}, secret_key);
}

[[nodiscard]] NCExpEncToServer nc_gen_result_message(NCNodeID const& node_id, std::vector<uint8_t> const& new_data, std::string const& secret_key) {
    /*
    Generate a result message to be sent from the node to the server.

    This message is only sent when the node has finished processing the data and sends
    the result back to the server.
    The secret key is used to encode the message.
    */

    return nc_encode_message_to_server(NCMessageType::NewResultFromNode, node_id, new_data, secret_key);
}

[[nodiscard]] NCExpEncToServer nc_gen_need_more_data_message(NCNodeID const& node_id, std::string const& secret_key) {
    /*
    Generate a "need more data" message to be sent from the node to the server.

    This message is only sent when the node has finished processing the data and needs
    more data to be processed from the server.
    The secret key is used to encode the message.
    */

    return nc_encode_message_to_server(NCMessageType::NodeNeedsMoreData, node_id, {}, secret_key);
}

[[nodiscard]] NCExpEncToNode nc_gen_new_data_message(std::vector<uint8_t> const& new_data, std::string const& secret_key) {
    /*
    Generate a "new data" message to be sent from the server to the node.

    This message is only sent when the node has asked for more data from the server.
    The secret key is used to encode the message.
    */

    return nc_encode_message_to_node(NCMessageType::NewDataFromServer, new_data, secret_key);
}

[[nodiscard]] NCExpEncToNode nc_gen_result_ok_message(std::string const& secret_key) {
    /*
    Generate a "result ok" message to be sent from the server to the node.

    This message is only sent when the node has sent processed data to the server
    and the server has accepted it.
    The secret key is used to encode the message.
    */

    return nc_encode_message_to_node(NCMessageType::ResultOK, {}, secret_key);
}

[[nodiscard]] NCExpEncToNode nc_gen_quit_message(std::string const& secret_key) {
    /*
    Generate a quit message to be sent from the server to the node.

    This message is only sent when the job is done and no more data has to be
    processed by the nodes.
    When receiving this message, the nodes will quit immediately.
    The server will wait some more time since not all nodes may have received
    the quit message yet.
    */

    return nc_encode_message_to_node(NCMessageType::Quit, {}, secret_key);
}
