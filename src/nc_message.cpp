/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines the message type and decoding / encoding
*/

// STD includes:
#include <cstring>

// Local includes:
#include "nc_compression.hpp"
#include "nc_encryption.hpp"
#include "nc_message.hpp"

std::expected<NCEncodedMessage, NCMessageError> nc_encode_message(NCMessageType const msg_type, std::string const& node_id, std::vector<uint8_t> const& data, std::string const& secret_key) {
    NCDecompressedMessage decompressed_message;
    decompressed_message.data = std::vector<uint8_t>(2 + node_id.size() + data.size());

    // 1. Encode message:
    // Encode message type (1 byte)
    decompressed_message.data.push_back(static_cast<uint8_t>(msg_type));

    // Encode node id, if present (NC_ID_LENGTH bytes):
    if (node_id.size() > 0) {
        decompressed_message.data.push_back(1);
        for (uint8_t const v: node_id) {
            decompressed_message.data.push_back(v);
        }
    } else {
        decompressed_message.data.push_back(0);
    }

    // Encode the actual data:
    if (data.size() > 0) {
        for (uint8_t const v: data) {
            decompressed_message.data.push_back(v);
        }
    }

    // 2. Compress message:
    std::expected<NCCompressedMessage, NCMessageError> compressed_message = nc_compress_message(decompressed_message);
    if (!compressed_message) {
        return std::unexpected(compressed_message.error());
    }

    // 3. Encrypt message:
    // NCDecryptedMessage decr_message{compressed_message->data};
    std::expected<NCEncryptedMessage, NCMessageError> encrypted_message = nc_encrypt_message(NCDecryptedMessage{compressed_message->data}, secret_key);
    if (!encrypted_message) {
        return std::unexpected(encrypted_message.error());
    }

    NCEncodedMessage result;
    result.data = std::vector<uint8_t>(NC_NONCE_LENGTH + NC_GCM_TAG_LENGTH + encrypted_message->data.size());
    for (uint8_t const v: encrypted_message->nonce) {
        result.data.push_back(v);
    }
    for (uint8_t const v: encrypted_message->tag) {
        result.data.push_back(v);
    }
    for (uint8_t const v: encrypted_message->data) {
        result.data.push_back(v);
    }

    return result;
}

std::expected<NCDecodedMessage, NCMessageError> nc_decode_message(NCEncodedMessage const& message, std::string const& secret_key) {
    // 1. Decrypt message:
    NCEncryptedMessage encrypted_message;
    uint32_t source_index = 0;
    uint32_t source_end = static_cast<uint32_t>(message.data.size());

    // Get nonce:
    for (uint8_t &v: encrypted_message.nonce) {
        v = message.data[source_index++];
    }
    // auto nonce_start = message.data.cbegin();
    // std::copy_n(nonce_start, NC_NONCE_LENGTH, encrypted_message.nonce.begin());

    // Get tag:
    for (uint8_t &v: encrypted_message.tag) {
        v = message.data[source_index++];
    }
    // auto tag_start = nonce_start + NC_NONCE_LENGTH;
    // std::copy_n(tag_start, NC_GCM_TAG_LENGTH, encrypted_message.tag.begin());

    // Get the rest of the data, if any:
    if (source_index < source_end) {
        encrypted_message.data = std::vector<uint8_t>(source_end - source_index);
        while (source_index < source_end) {
            encrypted_message.data.push_back(message.data[source_index++]);
        }
    }
    // auto data_start = tag_start + NC_GCM_TAG_LENGTH;
    // encrypted_message.data.assign(data_start, message.data.cend());

    std::expected<NCDecryptedMessage, NCMessageError> decrypted_message = nc_decrypt_message(encrypted_message, secret_key);
    if (!decrypted_message) {
        return std::unexpected(decrypted_message.error());
    }

    // 2. Decompress message:
    // NCCompressedMessage comp_message{decrypted_message->data};
    std::expected<NCDecompressedMessage, NCMessageError> decompressed_message = nc_decompress_message(NCCompressedMessage{decrypted_message->data});
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
        result.data = std::vector<uint8_t>(source_end - source_index);
        while (source_index < source_end) {
            result.data.push_back(decompressed_message->data[source_index++]);
        }
    }

    /*
    auto data_start = decompressed_message->data.cbegin() + 2;

    // Decode node id if it hase one:
    if (has_node_id == 1) {
        // Does have a node id:
        auto id_start = decompressed_message->data.cbegin() + 2;
        std::copy_n(id_start, NC_ID_LENGTH, result.node_id.id.begin());
        data_start = id_start + NC_ID_LENGTH;
    }

    // Decode the actual data:
    result.data.assign(data_start, decompressed_message->data.cend());
    */

    return result;
}

/*
[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_heartbeat_message(NCNodeID node_id, std::string secret_key) {

}

[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_heartbeat_message_ok(std::string secret_key) {

}

[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_heartbeat_message_error(std::string secret_key) {

}

[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_init_message(NCNodeID node_id, std::string secret_key) {

}

[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_init_message_ok(std::vector<uint8_t>, std::string secret_key) {

}

[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_init_message_error(std::string secret_key) {

}

[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_result_message(NCNodeID node_id, std::string secret_key, std::vector<uint8_t> new_data) {

}

[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_need_more_data_message(NCNodeID node_id, std::string secret_key) {

}

[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_new_data_message(std::vector<uint8_t> new_data, std::string secret_key) {

}

[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_result_ok_message(std::string secret_key) {

}

[[nodiscard]] std::expected<NCEncodedMessage, NCMessageError> nc_gen_quit_message(std::string secret_key) {

}
*/
