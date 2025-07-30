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
        return std::unexpected(NCMessageError::NCSizeMissmatch);
    }

    //std::cout << "decompressed_message size: " << decompressed_message.data.size() << "\n";

    // 2. Compress message:
    std::expected<NCCompressedMessage, NCMessageError> compressed_message = nc_compress_message(decompressed_message);
    if (!compressed_message) {
        return std::unexpected(compressed_message.error());
    }

    //std::cout << "compressed_message size: " << compressed_message->data.size() << "\n";

    // 3. Encrypt message:
    std::expected<NCEncryptedMessage, NCMessageError> encrypted_message = nc_encrypt_message(NCDecryptedMessage{compressed_message->data}, secret_key);
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
        return std::unexpected(NCMessageError::NCSizeMissmatch);
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

    //std::cout << "source_index: " << source_index << "\n";
    //std::cout << "encrypted data size: " << encrypted_message.data.size() << "\n";

    std::expected<NCDecryptedMessage, NCMessageError> decrypted_message = nc_decrypt_message(encrypted_message, secret_key);
    if (!decrypted_message) {
        return std::unexpected(decrypted_message.error());
    }

    // 2. Decompress message:
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
        result.data = std::vector<uint8_t>();
        result.data.reserve(source_end - source_index);
        while (source_index < source_end) {
            result.data.push_back(decompressed_message->data[source_index++]);
        }
    }

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
