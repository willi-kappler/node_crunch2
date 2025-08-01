/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines the message type and decoding / encoding
*/

// STD includes:
#include <type_traits>

// Local includes:
#include "nc_compression.hpp"
#include "nc_encryption.hpp"
#include "nc_message.hpp"

template <typename RetType>
[[nodiscard]] std::expected<RetType, NCMessageError> nc_encode(NCMessageType const msg_type,
        std::string_view node_id, std::vector<uint8_t> const& data, std::string const& secret_key) {
    // 1. Encode message:
    NCDecompressedMessage decompressed_message;

    if constexpr (std::is_same_v<RetType, NCEncodedMessageToServer>) {
        uint32_t const dc_size = static_cast<uint32_t>(1 + NC_NODEID_LENGTH + data.size());
        decompressed_message.data = std::vector<uint8_t>(dc_size);
    } else {
        uint32_t const dc_size = static_cast<uint32_t>(1 + data.size());
        decompressed_message.data = std::vector<uint8_t>(dc_size);
    }

    // Encode message type (1 byte)
    decompressed_message.data[0] = static_cast<uint8_t>(msg_type);
    auto dm_begin = decompressed_message.data.begin() + 1;

    if constexpr (std::is_same_v<RetType, NCEncodedMessageToServer>) {
        // Node id has to be encoded here:
        std::copy(node_id.cbegin(), node_id.cend(), dm_begin);
        dm_begin += NC_NODEID_LENGTH;
    }

    // Encode the actual data, if there is any:
    std::copy(data.cbegin(), data.cend(), dm_begin);

    // 2. Compress message:
    return nc_compress_message(decompressed_message).and_then([&secret_key](NCCompressedMessage compressed_message){
        // 3. Encrypt compressed message:
        return nc_encrypt_message(NCDecryptedMessage{compressed_message.data}, secret_key);
    }).transform([](NCEncryptedMessage encrypted_message){
        // 4. Encode encrypted compressed message:
        RetType result;
        uint32_t const result_size = NC_NONCE_LENGTH + NC_GCM_TAG_LENGTH + static_cast<uint32_t>(encrypted_message.data.size());
        result.data = std::vector<uint8_t>(result_size);
        auto const r_begin1 = result.data.begin();
        auto const r_begin2 = r_begin1 + NC_NONCE_LENGTH;
        auto const r_begin3 = r_begin2 + NC_GCM_TAG_LENGTH;

        // Encode nonce:
        std::copy(encrypted_message.nonce.cbegin(), encrypted_message.nonce.cend(), r_begin1);
        // Encode tag:
        std::copy(encrypted_message.tag.cbegin(), encrypted_message.tag.cend(), r_begin2);
        // Encode rest of message, if any:
        std::copy(encrypted_message.data.cbegin(), encrypted_message.data.cend(), r_begin3);

        return result;
    });
}

template <typename RetType>
[[nodiscard]] std::expected<RetType, NCMessageError> nc_decode(std::vector<uint8_t> const& message,
        std::string const& secret_key) {
    // 1. Decrypt message:
    NCEncryptedMessage encrypted_message;
    auto const m_begin1 = message.cbegin();
    auto const m_begin2 = m_begin1 + NC_NONCE_LENGTH;
    auto const m_begin3 = m_begin2 + NC_GCM_TAG_LENGTH;

    // Decode nonce:
    std::copy(m_begin1, m_begin2, encrypted_message.nonce.begin());
    // Decode tag:
    std::copy(m_begin2, m_begin3, encrypted_message.tag.begin());
    // Decode the rest of the data, if any:
    encrypted_message.data = std::vector<uint8_t>(m_begin3, message.cend());

    // 2. Decrypt message:
    return nc_decrypt_message(encrypted_message, secret_key).and_then([](NCDecryptedMessage decrypted_message){
        // 3. Decompress decrpted message:
        return nc_decompress_message(NCCompressedMessage{decrypted_message.data});
    }).transform([](NCDecompressedMessage decompressed_message){
        // 4. Decode message:
        RetType result;
        // Decode message type:
        result.msg_type = static_cast<NCMessageType>(decompressed_message.data[0]);
        auto m_begin = decompressed_message.data.cbegin() + 1;

        if constexpr (std::is_same_v<RetType, NCDecodedMessageFromNode>) {
            // Decode node id:
            std::copy(m_begin, m_begin + NC_NODEID_LENGTH, result.node_id.id.begin());
            m_begin += NC_NODEID_LENGTH;
        }

        // Decode the actual data, if any:
        result.data = std::vector<uint8_t>(m_begin, decompressed_message.data.cend());

        return result;
    });
}

// Explicit instantiations, so that the linker can find the functions:
template [[nodiscard]] NCExpEncToNode nc_encode<NCEncodedMessageToNode>(
    NCMessageType const msg_type, std::string_view node_id,
    std::vector<uint8_t> const& data, std::string const& secret_key);

template [[nodiscard]] NCExpEncToServer nc_encode<NCEncodedMessageToServer>(
    NCMessageType const msg_type, std::string_view node_id,
    std::vector<uint8_t> const& data, std::string const& secret_key);

template [[nodiscard]] NCExpDecFromNode nc_decode<NCDecodedMessageFromNode>(
    std::vector<uint8_t> const& message, std::string const& secret_key);

template [[nodiscard]] NCExpDecFromServer nc_decode<NCDecodedMessageFromServer>(
    std::vector<uint8_t> const& message, std::string const& secret_key);

// Helper functions:
[[nodiscard]] NCExpEncToServer nc_encode_message_to_server(NCMessageType const msg_type,
        NCNodeID const& node_id, std::vector<uint8_t> const& data, std::string const& secret_key) {
    return nc_encode<NCEncodedMessageToServer>(msg_type, node_id.id, data, secret_key);
}

[[nodiscard]] NCExpEncToNode nc_encode_message_to_node(NCMessageType const msg_type,
        std::vector<uint8_t> const& data, std::string const& secret_key) {
    return nc_encode<NCEncodedMessageToNode>(msg_type, "", data, secret_key);
}

[[nodiscard]] NCExpDecFromServer nc_decode_message_from_server(NCEncodedMessageToNode const& message,
        std::string const& secret_key) {
    return nc_decode<NCDecodedMessageFromServer>(message.data, secret_key);
}

[[nodiscard]] NCExpDecFromNode nc_decode_message_from_node(NCEncodedMessageToServer const& message,
        std::string const& secret_key) {
    return nc_decode<NCDecodedMessageFromNode>(message.data, secret_key);
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

[[nodiscard]] NCExpEncToServer nc_gen_result_message(NCNodeID const& node_id,
        std::vector<uint8_t> const& new_data, std::string const& secret_key) {
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

[[nodiscard]] NCExpEncToNode nc_gen_new_data_message(std::vector<uint8_t> const& new_data,
        std::string const& secret_key) {
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
