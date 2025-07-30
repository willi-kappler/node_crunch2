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

std::expected<NCEncodedMessage, NCMessageError> nc_encode_message(NCMessageType const msg_type, std::string const& node_id, std::vector<uint8_t> const& data, std::string const& secret_key) {
    NCDecompressedMessage decompressed_message;
    uint32_t expected_size = static_cast<uint32_t>(2 + node_id.size() + data.size());
    decompressed_message.data = std::vector<uint8_t>(expected_size);
    //decompressed_message.data.reserve(expected_size);
    uint32_t data_index = 0;

    //std::cout << "decompressed_message capacity: " << decompressed_message.data.capacity() << "\n";

    // 1. Encode message:
    // Encode message type (1 byte)
    decompressed_message.data[data_index++] = static_cast<uint8_t>(msg_type);

    // Encode node id, if present (NC_NODEID_LENGTH bytes):
    if (node_id.size() > 0) {
        decompressed_message.data[data_index++] = 1;
        for (uint8_t const v: node_id) {
            decompressed_message.data[data_index++] = v;
        }
    } else {
        decompressed_message.data[data_index++] = 0;
    }

    // Encode the actual data:
    if (data.size() > 0) {
        for (uint8_t const v: data) {
            decompressed_message.data[data_index++] = v;
        }
    }

    if (expected_size != decompressed_message.data.size()) {
        return std::unexpected(NCMessageError::SizeMissmatch);
    }

    //std::cout << "decompressed_message size: " << decompressed_message.data.size() << "\n";

    // 2. Compress message:
    auto compressed_message = nc_compress_message(decompressed_message);
    if (!compressed_message) {
        return std::unexpected(compressed_message.error());
    }

    //std::cout << "compressed_message size: " << compressed_message->data.size() << "\n";

    // 3. Encrypt message:
    auto encrypted_message = nc_encrypt_message(NCDecryptedMessage{compressed_message->data}, secret_key);
    if (!encrypted_message) {
        return std::unexpected(encrypted_message.error());
    }

    //std::cout << "encrypted_message size: " << encrypted_message->data.size() << "\n";

    NCEncodedMessage result;
    // result.data = std::vector<uint8_t>();
    expected_size = static_cast<uint32_t>(NC_NONCE_LENGTH + NC_GCM_TAG_LENGTH + encrypted_message->data.size());
    result.data.reserve(expected_size);

    //std::cout << "result capacity: " << result.data.capacity() << "\n";

    for (uint8_t const v: encrypted_message->nonce) {
        result.data.push_back(v);
    }
    for (uint8_t const v: encrypted_message->tag) {
        result.data.push_back(v);
    }
    for (uint8_t const v: encrypted_message->data) {
        result.data.push_back(v);
    }

    //std::cout << "nonce size: " << static_cast<uint32_t>(NC_NONCE_LENGTH) << "\n";
    //std::cout << "tag size: " << static_cast<uint32_t>(NC_GCM_TAG_LENGTH) << "\n";
    //std::cout << "result size: " << result.data.size() << "\n";

    if (result.data.size() != expected_size) {
        return std::unexpected(NCMessageError::SizeMissmatch);
    }

    return result;
}

std::expected<NCDecodedMessage, NCMessageError> nc_decode_message(NCEncodedMessage const& message, std::string const& secret_key) {
    // 1. Decrypt message:
    NCEncryptedMessage encrypted_message;
    uint32_t source_index = 0;
    uint32_t source_end = static_cast<uint32_t>(message.data.size());

    //std::cout << "message size: " << source_end << "\n";

    // Get nonce:
    for (uint8_t &v: encrypted_message.nonce) {
        v = message.data[source_index++];
    }

    //std::cout << "source_index: " << source_index << "\n";

    // Get tag:
    for (uint8_t &v: encrypted_message.tag) {
        v = message.data[source_index++];
    }

    //std::cout << "source_index: " << source_index << "\n";

    // Get the rest of the data, if any:
    if (source_index < source_end) {
        encrypted_message.data = std::vector<uint8_t>();
        encrypted_message.data.reserve(source_end - source_index);
        while (source_index < source_end) {
            encrypted_message.data.push_back(message.data[source_index++]);
        }
    }

    if (source_index != source_end) {
        return std::unexpected(NCMessageError::SizeMissmatch);
    }

    //std::cout << "source_index: " << source_index << "\n";
    //std::cout << "encrypted data size: " << encrypted_message.data.size() << "\n";

    auto decrypted_message = nc_decrypt_message(encrypted_message, secret_key);
    if (!decrypted_message) {
        return std::unexpected(decrypted_message.error());
    }

    // 2. Decompress message:
    auto decompressed_message = nc_decompress_message(NCCompressedMessage{decrypted_message->data});
    if (!decompressed_message) {
        return std::unexpected(decompressed_message.error());
    }

    // 3. Decode message:
    NCDecodedMessage result;
    // Decode message type:
    result.msg_type = static_cast<NCMessageType>(decompressed_message->data[0]);
    uint8_t has_node_id = decompressed_message->data[1];
    source_index = 2;
    source_end = static_cast<uint32_t>(decompressed_message->data.size());

    // Decode node id if it hase one:
    if (has_node_id == 1) {
        for (auto &v: result.node_id.id) {
            v = decompressed_message->data[source_index++];
        }
    }

    // Decode the actual data, if any:
    if (source_index < source_end) {
        result.data = std::vector<uint8_t>();
        result.data.reserve(source_end - source_index);
        while (source_index < source_end) {
            result.data.push_back(decompressed_message->data[source_index++]);
        }
    }

    if (source_index != source_end) {
        return std::unexpected(NCMessageError::SizeMissmatch);
    }

    return result;
}

[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_heartbeat_message(NCNodeID const node_id, std::string const& secret_key) {
    /*
    Generate a heartbeat message to be sent from the node to the server.

    The node sends its node_id that the server will check.
    The secret key is used to encode the message.
    */

    return nc_encode_message(NCMessageType::Heartbeat, node_id.id, {}, secret_key);
}

[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_heartbeat_message_ok(std::string const& secret_key) {
    /*
    Generate a "heartbeat OK" message to be sent from the server to the node.

    This message is only sent if the heartbeat message from the node was
    sent in the time limit and contained a valid node id.
    The secret key is used to encode the message.
    */

    return nc_encode_message(NCMessageType::HeartbeatOK, "", {}, secret_key);
}

[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_heartbeat_message_error(std::string const& secret_key) {
    /*
    Generate a "heartbeat error" message to be sent from the server to the node.

    This message is only sent if the heartbeat message from the node was
    sent too late or did not contain a valid node id.
    The secret key is used to encode the message.
    */

    return nc_encode_message(NCMessageType::HeartbeatError, "", {}, secret_key);
}

[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_init_message(NCNodeID const node_id, std::string const& secret_key) {
    /*
    Generate an initialisation message to be sent from the node to the server.

    This message is only sent once when the node connects for the first time to the server.
    The node registers itself to the server given its own node id.
    The secret key is used to encode the message.
    */

    return nc_encode_message(NCMessageType::Init, node_id.id, {}, secret_key);
}

[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_init_message_ok(std::vector<uint8_t> const& init_data, std::string const& secret_key) {
    /*
    Generate an "init ok" message to be sent from the server to the node.

    This message is only sent once when the node has registered itself correctly to the server.
    The server then can send some initial data to the node, if needed.
    The secret key is used to encode the message.
    */

    return nc_encode_message(NCMessageType::InitOK, "", init_data, secret_key);
}

[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_init_message_error(std::string const& secret_key) {
    /*
    Generate an "init error" message to be sent from the server to the node.

    This message is only sent when the registration of the new node has failed.
    The secret key is used to encode the message.
    */

    return nc_encode_message(NCMessageType::InitError, "", {}, secret_key);
}

[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_result_message(NCNodeID const node_id, std::vector<uint8_t> const& new_data, std::string const& secret_key) {
    /*
    Generate a result message to be sent from the node to the server.

    This message is only sent when the node has finished processing the data and sends
    the result back to the server.
    The secret key is used to encode the message.
    */

    return nc_encode_message(NCMessageType::NewResultFromNode, node_id.id, new_data, secret_key);
}

[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_need_more_data_message(NCNodeID const node_id, std::string const& secret_key) {
    /*
    Generate a "need more data" message to be sent from the node to the server.

    This message is only sent when the node has finished processing the data and needs
    more data to be processed from the server.
    The secret key is used to encode the message.
    */

    return nc_encode_message(NCMessageType::NodeNeedsMoreData, node_id.id, {}, secret_key);
}

[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_new_data_message(std::vector<uint8_t> const& new_data, std::string const& secret_key) {
    /*
    Generate a "new data" message to be sent from the server to the node.

    This message is only sent when the node has asked for more data from the server.
    The secret key is used to encode the message.
    */

    return nc_encode_message(NCMessageType::NewDataFromServer, "", new_data, secret_key);
}

[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_result_ok_message(std::string const& secret_key) {
    /*
    Generate a "result ok" message to be sent from the server to the node.

    This message is only sent when the node has sent processed data to the server
    and the server has accepted it.
    The secret key is used to encode the message.
    */

    return nc_encode_message(NCMessageType::ResultOK, "", {}, secret_key);
}

[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_quit_message(std::string const& secret_key) {
    /*
    Generate a quit message to be sent from the server to the node.

    This message is only sent when the job is done and no more data has to be
    processed by the nodes.
    When receiving this message, the nodes will quit immediately.
    The server will wait some more time since not all nodes may have received
    the quit message yet.
    */

    return nc_encode_message(NCMessageType::Quit, "", {}, secret_key);
}
