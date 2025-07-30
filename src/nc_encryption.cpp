/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines encryption of messages
*/

// STD includes:
# include <iostream>

// External includes:
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rand.h>

// Local includes:
#include "nc_encryption.hpp"

void nc_print_tag(std::vector<uint8_t> const& tag) {
    std::cout << "Tag: " << std::endl;
    for (auto v: tag) {
        std::cout << std::hex << static_cast<uint16_t>(v) << ":";
    }
    std::cout << std::dec << std::endl;
}

void nc_print_nonce(std::vector<uint8_t> const& nonce) {
    std::cout << "Nonce: " << std::endl;
    for (auto v: nonce) {
        std::cout << std::hex << static_cast<uint16_t>(v) << ":";
    }
    std::cout << std::dec << std::endl;
}

std::expected<NCEncryptedMessage, NCMessageError> nc_encrypt_message(NCDecryptedMessage const& message, std::string const& secret_key) {
    EVP_CIPHER_CTX* ctx = nullptr;

    // Create and initialize context:
    if (!(ctx = EVP_CIPHER_CTX_new())) {
        return std::unexpected(NCMessageError::CipherContextError);
    }

    // Initialize the encryption operation:
    if (1 != EVP_EncryptInit_ex(ctx, EVP_chacha20_poly1305(), nullptr, reinterpret_cast<const unsigned char *>(secret_key.c_str()), nullptr)) {
        EVP_CIPHER_CTX_free(ctx);
        return std::unexpected(NCMessageError::EncryptInitError);
    }

    // Set the nonce (IV) length. ChaCha20-Poly1305 uses 12-byte nonce:
    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_IVLEN, NC_NONCE_LENGTH, nullptr)) {
        EVP_CIPHER_CTX_free(ctx);
        return std::unexpected(NCMessageError::CipherControllError);
    }

    // The encoded message:
    NCEncryptedMessage result;

    // 96-bit (12 bytes) nonce (IV) - MUST be unique for each encryption with the same key:
    if (1 != RAND_bytes(result.nonce.data(), NC_NONCE_LENGTH)) {
        EVP_CIPHER_CTX_free(ctx);
        return std::unexpected(NCMessageError::CreateNonceError);
    }

    // nc_print_nonce(result.nonce);

    // Set the nonce (IV):
    if (1 != EVP_EncryptInit_ex(ctx, nullptr, nullptr, nullptr, result.nonce.data())) {
        EVP_CIPHER_CTX_free(ctx);
        return std::unexpected(NCMessageError::SetNonceError);
    }

    int32_t block_size = EVP_CIPHER_get_block_size(EVP_chacha20_poly1305());
    // std::cout << "Block size: " << std::dec << block_size << std::endl;

    // Provide the message data to be encrypted:
    int32_t len = 0;
    uint32_t const message_len = static_cast<uint32_t>(message.data.size());
    // std::cout << "Message length: " << message_len << std::endl;
    // Ciphertext will be same size as message:
    result.data.resize(message_len + block_size);
    if (1 != EVP_EncryptUpdate(ctx, result.data.data(), &len, message.data.data(), message_len)) {
        EVP_CIPHER_CTX_free(ctx);
        return std::unexpected(NCMessageError::EncryptUpdateError);
    }
    uint32_t ciphertext_len = len;
    // std::cout << "Ciphertext length 1: " << ciphertext_len << std::endl;

    // Finalize the encryption. This also generates the authentication tag:
    if (1 != EVP_EncryptFinal_ex(ctx, result.data.data() + len, &len)) {
        EVP_CIPHER_CTX_free(ctx);
        return std::unexpected(NCMessageError::EncryptFinalError);
    }
    // Adjust size in case of any padding (though AEAD stream ciphers generally don't pad):
    ciphertext_len += len;
    // std::cout << "Ciphertext length 2: " << ciphertext_len << std::endl;
    result.data.resize(ciphertext_len);

    // Get the authentication tag. Poly1305 tag is 16 bytes:
    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_GET_TAG, NC_GCM_TAG_LENGTH, result.tag.data())) {
        EVP_CIPHER_CTX_free(ctx);
        return std::unexpected(NCMessageError::CipherGetTagError);
    }

    // nc_print_tag(result.tag);

    EVP_CIPHER_CTX_free(ctx);
    return result;
}

std::expected<NCDecryptedMessage, NCMessageError> nc_decrypt_message(NCEncryptedMessage const& message, std::string const& secret_key) {
    EVP_CIPHER_CTX* ctx = nullptr;

    // Create and initialize context:
    if (!(ctx = EVP_CIPHER_CTX_new())) {
        return std::unexpected(NCMessageError::CipherContextError);
    }

    // Initialize the decryption operation
    if (1 != EVP_DecryptInit_ex(ctx, EVP_chacha20_poly1305(), nullptr, reinterpret_cast<const unsigned char *>(secret_key.c_str()), nullptr)) {
        EVP_CIPHER_CTX_free(ctx);
        return std::unexpected(NCMessageError::DencryptInitError);
    }

    // Set the nonce (IV) length. ChaCha20-Poly1305 uses 12-byte nonce:
    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_IVLEN, NC_NONCE_LENGTH, nullptr)) {
        EVP_CIPHER_CTX_free(ctx);
        return std::unexpected(NCMessageError::CipherControllError);
    }

    // nc_print_nonce(message.nonce);

    // Set the nonce (IV):
    if (1 != EVP_DecryptInit_ex(ctx, nullptr, nullptr, nullptr, message.nonce.data())) {
        EVP_CIPHER_CTX_free(ctx);
        return std::unexpected(NCMessageError::SetNonceError);
    }

    // nc_print_tag(message.tag);

    // Set the expected authentication tag. This must be done BEFORE processing ciphertext:
    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_TAG, NC_GCM_TAG_LENGTH, (void*) (message.tag.data()))) {
        EVP_CIPHER_CTX_free(ctx);
        return std::unexpected(NCMessageError::CipherSetTagError);
    }

    int32_t const block_size = EVP_CIPHER_get_block_size(EVP_chacha20_poly1305());

    // std::cout << "block_size: " << block_size << "\n";

    // Provide the ciphertext data to be decrypted:
    NCDecryptedMessage result;
    uint32_t const ciphertext_len = static_cast<uint32_t>(message.data.size());
     // Plaintext will be same size as ciphertext:
    result.data.resize(ciphertext_len + block_size);
    int32_t len = 0;
    if (1 != EVP_DecryptUpdate(ctx, result.data.data(), &len, message.data.data(), ciphertext_len)) {
        EVP_CIPHER_CTX_free(ctx);
        return std::unexpected(NCMessageError::DecryptUpdateError);
    }

    uint32_t plaintext_len = len;

    // std::cout << "ciphertext_len: " << ciphertext_len << "\n";
    // std::cout << "plaintext_len: " << plaintext_len << "\n";

    // Finalize the decryption. This performs the tag verification.
    // If the tag is incorrect, this function will return 0:
    if (1 != EVP_DecryptFinal_ex(ctx, result.data.data() + len, &len)) {
        EVP_CIPHER_CTX_free(ctx);
        return std::unexpected(NCMessageError::DecryptFinalError);
    }
    plaintext_len += len;

    // std::cout << "plaintext_len: " << plaintext_len << "\n";

    // Adjust size:
    result.data.resize(plaintext_len);
    EVP_CIPHER_CTX_free(ctx);

    return result;
}
