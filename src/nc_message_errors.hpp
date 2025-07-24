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
    NCCompressionError,
    NCDecompressionError,
    NCEncryptionError,
    NCDecryptionError
};

#endif // FILE_NC_MESSAGE_ERRORS_HPP_INCLUDED