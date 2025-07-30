/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines message errors
*/

#ifndef FILE_NC_MESSAGE_ERRORS_HPP_INCLUDED
#define FILE_NC_MESSAGE_ERRORS_HPP_INCLUDED

// STD includes:
#include <cstdint>

enum struct NCMessageError: uint8_t {
    CipherContextError = 0,
    EncryptInitError,
    DencryptInitError,
    CipherControllError,
    CreateNonceError,
    SetNonceError,
    EncryptUpdateError,
    DecryptUpdateError,
    EncryptFinalError,
    DecryptFinalError,
    CipherGetTagError,
    CipherSetTagError,
    CompressionError,
    DecompressionError,
    EncryptionError,
    DecryptionError,
    SizeMissmatch
};

#endif // FILE_NC_MESSAGE_ERRORS_HPP_INCLUDED