/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines the message type and decoding / encoding
*/

// STD includes:
#include <type_traits>

// Local includes:
#include "nc_message.hpp"

namespace NodeCrunch2 {
NCMessageCodecBase::NCMessageCodecBase(std::string const secret_key):
    compressor_intern(),
    encryption_intern(NCEncryption(secret_key)) {}

NCMessageCodecBase::NCMessageCodecBase(NCCompressor nc_compressor, NCEncryption nc_encryption):
    compressor_intern(nc_compressor),
    encryption_intern(nc_encryption) {}

[[nodiscard]] std::vector<uint8_t> NCMessageCodecBase::nc_encode(NCDecompressedMessage decompressed_message) const {
        // 2. Compress message:
        NCCompressedMessage compressed_message = compressor_intern.nc_compress_message(decompressed_message);
        // 3. Encrypt compressed message:
        NCEncryptedMessage encrypted_message = encryption_intern.nc_encrypt_message(NCDecryptedMessage{compressed_message.data});
        // 4. Encode encrypted compressed message:
        std::vector<uint8_t> result;
        uint32_t const result_size = NC_NONCE_LENGTH + NC_GCM_TAG_LENGTH + static_cast<uint32_t>(encrypted_message.data.size());
        result = std::vector<uint8_t>(result_size);
        auto const r_begin1 = result.begin();
        auto const r_begin2 = r_begin1 + NC_NONCE_LENGTH;
        auto const r_begin3 = r_begin2 + NC_GCM_TAG_LENGTH;

        // Encode nonce:
        std::copy(encrypted_message.nonce.cbegin(), encrypted_message.nonce.cend(), r_begin1);
        // Encode tag:
        std::copy(encrypted_message.tag.cbegin(), encrypted_message.tag.cend(), r_begin2);
        // Encode rest of message, if any:
        std::copy(encrypted_message.data.cbegin(), encrypted_message.data.cend(), r_begin3);

        return result;
}

[[nodiscard]] NCDecompressedMessage NCMessageCodecBase::nc_decode(std::vector<uint8_t> const& message) const {
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
    NCDecryptedMessage decrypted_message = encryption_intern.nc_decrypt_message(encrypted_message);
    // 3. Decompress decrpted message:
    NCDecompressedMessage decompressed_message = compressor_intern.nc_decompress_message(NCCompressedMessage{decrypted_message.data});

    return decompressed_message;
}

NCMessageCodecNode::NCMessageCodecNode(NCNodeID const node_id, std::string const secret_key):
    NCMessageCodecBase(secret_key),
    node_id_intern(node_id) {}

NCMessageCodecNode::NCMessageCodecNode(NCNodeID const node_id, NCCompressor nc_compressor, NCEncryption nc_encryption):
    NCMessageCodecBase(nc_compressor, nc_encryption),
    node_id_intern(node_id) {}

[[nodiscard]] NCEncodedMessageToServer NCMessageCodecNode::nc_encode_message_to_server(
    NCMessageType const msg_type, std::vector<uint8_t> const& data) const {
    // 1. Encode message:
    NCDecompressedMessage decompressed_message;

    uint32_t const dc_size = static_cast<uint32_t>(1 + NC_NODEID_LENGTH + data.size());
    decompressed_message.data = std::vector<uint8_t>(dc_size);

    // Encode message type (1 byte)
    decompressed_message.data[0] = static_cast<uint8_t>(msg_type);
    auto dm_begin = decompressed_message.data.begin() + 1;

    // Node id has to be encoded here:
    std::copy(node_id_intern.id.cbegin(), node_id_intern.id.cend(), dm_begin);
    dm_begin += NC_NODEID_LENGTH;

    // Encode the actual data, if there is any:
    std::copy(data.cbegin(), data.cend(), dm_begin);

    // Steps 2 to 4:
    std::vector<uint8_t> encoded = nc_encode(decompressed_message);
    return NCEncodedMessageToServer{encoded};
}

[[nodiscard]] NCDecodedMessageFromServer NCMessageCodecNode::nc_decode_message_from_server(
    NCEncodedMessageToNode const& message) const {
    // Steps 1 to 3:
    NCDecompressedMessage decompressed_message = nc_decode(message.data);

    // 4. Decode message:
    NCDecodedMessageFromServer result;
    // Decode message type:
    result.msg_type = static_cast<NCMessageType>(decompressed_message.data[0]);
    auto m_begin = decompressed_message.data.cbegin() + 1;

    // Decode the actual data, if any:
    result.data = std::vector<uint8_t>(m_begin, decompressed_message.data.cend());

    return result;
}

[[nodiscard]] NCEncodedMessageToServer NCMessageCodecNode::nc_gen_heartbeat_message() const {
    /*
    Generate a heartbeat message to be sent from the node to the server.

    The node sends its node_id that the server will check.
    The secret key is used to encode the message.
    */

    return nc_encode_message_to_server(NCMessageType::Heartbeat, {});
}

[[nodiscard]] NCEncodedMessageToServer NCMessageCodecNode::nc_gen_init_message() const {
    /*
    Generate an initialisation message to be sent from the node to the server.

    This message is only sent once when the node connects for the first time to the server.
    The node registers itself to the server given its own node id.
    The secret key is used to encode the message.
    */

    return nc_encode_message_to_server(NCMessageType::Init, {});
}

[[nodiscard]] NCEncodedMessageToServer NCMessageCodecNode::nc_gen_result_message(
    std::vector<uint8_t> const& new_data) const {
    /*
    Generate a result message to be sent from the node to the server.

    This message is only sent when the node has finished processing the data and sends
    the result back to the server.
    The secret key is used to encode the message.
    */

    return nc_encode_message_to_server(NCMessageType::NewResultFromNode, new_data);
}

[[nodiscard]] NCEncodedMessageToServer NCMessageCodecNode::nc_gen_need_more_data_message() const {
    /*
    Generate a "need more data" message to be sent from the node to the server.

    This message is only sent when the node has finished processing the data and needs
    more data to be processed from the server.
    The secret key is used to encode the message.
    */

    return nc_encode_message_to_server(NCMessageType::NodeNeedsMoreData, {});
}

NCMessageCodecServer::NCMessageCodecServer(std::string const secret_key):
    NCMessageCodecBase(secret_key) {}

NCMessageCodecServer::NCMessageCodecServer(NCCompressor nc_compressor2, NCEncryption nc_encryption2):
    NCMessageCodecBase(nc_compressor2, nc_encryption2) {}

[[nodiscard]] NCEncodedMessageToNode NCMessageCodecServer::nc_encode_message_to_node(
    NCMessageType const msg_type, std::vector<uint8_t> const& data) const {
    // 1. Encode message:
    NCDecompressedMessage decompressed_message;

    uint32_t const dc_size = static_cast<uint32_t>(1 + data.size());
    decompressed_message.data = std::vector<uint8_t>(dc_size);

    // Encode message type (1 byte)
    decompressed_message.data[0] = static_cast<uint8_t>(msg_type);
    auto dm_begin = decompressed_message.data.begin() + 1;

    // Encode the actual data, if there is any:
    std::copy(data.cbegin(), data.cend(), dm_begin);

    // Steps 2 to 4:
    std::vector<uint8_t> encoded = nc_encode(decompressed_message);
    return NCEncodedMessageToNode{encoded};
}

[[nodiscard]] NCDecodedMessageFromNode NCMessageCodecServer::nc_decode_message_from_node(
    NCEncodedMessageToServer const& message) const {
    // Steps 1 to 3:
    NCDecompressedMessage decompressed_message = nc_decode(message.data);

    // 4. Decode message:
    NCDecodedMessageFromNode result;
    // Decode message type:
    result.msg_type = static_cast<NCMessageType>(decompressed_message.data[0]);
    auto m_begin = decompressed_message.data.cbegin() + 1;

    // Decode node id:
    std::copy(m_begin, m_begin + NC_NODEID_LENGTH, result.node_id.id.begin());
    m_begin += NC_NODEID_LENGTH;

    // Decode the actual data, if any:
    result.data = std::vector<uint8_t>(m_begin, decompressed_message.data.cend());

    return result;
}

[[nodiscard]] NCEncodedMessageToNode NCMessageCodecServer::nc_gen_heartbeat_message_ok() const {
        /*
    Generate a "heartbeat OK" message to be sent from the server to the node.

    This message is only sent if the heartbeat message from the node was
    sent in the time limit and contained a valid node id.
    The secret key is used to encode the message.
    */

    return nc_encode_message_to_node(NCMessageType::HeartbeatOK, {});
}

[[nodiscard]] NCEncodedMessageToNode NCMessageCodecServer::nc_gen_heartbeat_message_error() const {
    /*
    Generate a "heartbeat error" message to be sent from the server to the node.

    This message is only sent if the heartbeat message from the node was
    sent too late or did not contain a valid node id.
    The secret key is used to encode the message.
    */

    return nc_encode_message_to_node(NCMessageType::HeartbeatError, {});
}

[[nodiscard]] NCEncodedMessageToNode NCMessageCodecServer::nc_gen_init_message_ok(
    std::vector<uint8_t> const& init_data) const {
    /*
    Generate an "init ok" message to be sent from the server to the node.

    This message is only sent once when the node has registered itself correctly to the server.
    The server then can send some initial data to the node, if needed.
    The secret key is used to encode the message.
    */

    return nc_encode_message_to_node(NCMessageType::InitOK, init_data);
}

[[nodiscard]] NCEncodedMessageToNode NCMessageCodecServer::nc_gen_init_message_error() const {
    /*
    Generate an "init error" message to be sent from the server to the node.

    This message is only sent when the registration of the new node has failed.
    The secret key is used to encode the message.
    */

    return nc_encode_message_to_node(NCMessageType::InitError, {});
}

[[nodiscard]] NCEncodedMessageToNode NCMessageCodecServer::nc_gen_new_data_message(
    std::vector<uint8_t> const& new_data) const {
    /*
    Generate a "new data" message to be sent from the server to the node.

    This message is only sent when the node has asked for more data from the server.
    The secret key is used to encode the message.
    */

    return nc_encode_message_to_node(NCMessageType::NewDataFromServer, new_data);
}

[[nodiscard]] NCEncodedMessageToNode NCMessageCodecServer::nc_gen_result_ok_message() const {
    /*
    Generate a "result ok" message to be sent from the server to the node.

    This message is only sent when the node has sent processed data to the server
    and the server has accepted it.
    The secret key is used to encode the message.
    */

    return nc_encode_message_to_node(NCMessageType::ResultOK, {});
}

[[nodiscard]] NCEncodedMessageToNode NCMessageCodecServer::nc_gen_quit_message() const {
    /*
    Generate a quit message to be sent from the server to the node.

    This message is only sent when the job is done and no more data has to be
    processed by the nodes.
    When receiving this message, the nodes will quit immediately.
    The server will wait some more time since not all nodes may have received
    the quit message yet.
    */

    return nc_encode_message_to_node(NCMessageType::Quit, {});
}
}