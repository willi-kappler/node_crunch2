/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines encryption of messages
*/

// STD includes:


// External includes:
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rand.h>

// Local includes:
#include "nc_encryption.hpp"

std::expected<NCEncodedMessage, NCMessageError> nc_encrypt_message(NCCompressedMessage const& message, std::string const& secret_key) {
    EVP_CIPHER_CTX* ctx = nullptr;

    // Create and initialize context:
    if (!(ctx = EVP_CIPHER_CTX_new())) {
        return std::unexpected(NCMessageError::NCCipherContextError);
    }

    // Initialize the encryption operation:
    if (1 != EVP_EncryptInit_ex(ctx, EVP_chacha20_poly1305(), nullptr, reinterpret_cast<const unsigned char *>(secret_key.c_str()), nullptr)) {
        EVP_CIPHER_CTX_free(ctx);
        return std::unexpected(NCMessageError::NCEncryptInitError);
    }

    // Set the nonce (IV) length. ChaCha20-Poly1305 uses 12-byte nonce:
    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_IVLEN, 12, nullptr)) {
        EVP_CIPHER_CTX_free(ctx);
        return std::unexpected(NCMessageError::NCCipherControllError);
    }

    // The encoded message:
    NCEncodedMessage result;

    // 96-bit (12 bytes) nonce (IV) - MUST be unique for each encryption with the same key:
    if (1 != RAND_bytes(result.nonce.data(), NC_NONCE_LENGTH)) {
        EVP_CIPHER_CTX_free(ctx);
        return std::unexpected(NCMessageError::NCCreateNonceError);
    }

    // Set the nonce (IV):
    if (1 != EVP_EncryptInit_ex(ctx, nullptr, nullptr, nullptr, result.nonce.data())) {
        EVP_CIPHER_CTX_free(ctx);
        return std::unexpected(NCMessageError::NCSetNonceError);
    }

    // Provide the message data to be encrypted:
    int32_t len = 0;
    uint32_t ciphertext_len = 0;
    uint32_t const message_len = message.data.size();
    // Ciphertext will be same size as message:
    result.data.resize(message_len);
    if (1 != EVP_EncryptUpdate(ctx, result.data.data(), &len, message.data.data(), message_len)) {
        EVP_CIPHER_CTX_free(ctx);
        return std::unexpected(NCMessageError::NCEncryptUpdateError);
    }
    ciphertext_len = len;

    // Finalize the encryption. This also generates the authentication tag:
    if (1 != EVP_EncryptFinal_ex(ctx, result.data.data() + len, &len)) {
        EVP_CIPHER_CTX_free(ctx);
        return std::unexpected(NCMessageError::NCEncryptFinalError);
    }
    // Adjust size in case of any padding (though AEAD stream ciphers generally don't pad):
    ciphertext_len += len;
    result.data.resize(ciphertext_len);

    // Get the authentication tag. Poly1305 tag is 16 bytes:
    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_GET_TAG, NC_GCM_TAG_LENGTH, result.tag.data())) {
        EVP_CIPHER_CTX_free(ctx);
        return std::unexpected(NCMessageError::NCCipherTagError);
    }

    EVP_CIPHER_CTX_free(ctx);
    return result;
}

std::expected<NCCompressedMessage, NCMessageError> nc_decrypt_message(NCEncodedMessage const& message, std::string const& secret_key) {
    // TODO

    if (secret_key.size() == 0) {

    }

    return NCCompressedMessage(message.data);
}
