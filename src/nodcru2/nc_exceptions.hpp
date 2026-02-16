/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines all the exceptions that are used in this code base.
*/

#ifndef FILE_NC_EXCEPTIONS_HPP_INCLUDED
#define FILE_NC_EXCEPTIONS_HPP_INCLUDED

#include <stdexcept>

namespace nodcru2 {
class NCInvalidKeyException: public std::invalid_argument {
public:
  NCInvalidKeyException(): std::invalid_argument("Size of secret key must be 32 bytes.") { }
};

class NCCompressionException: public std::runtime_error {
public:
  NCCompressionException(): std::runtime_error("Compression error.") { }
};

class NCDecompressionException: public std::runtime_error {
public:
  NCDecompressionException(): std::runtime_error("Decompression error.") { }
};

class NCEncryptionException: public std::runtime_error {
public:
  NCEncryptionException(const char *msg): std::runtime_error(msg) { }
};

class NCDecryptionException: public std::runtime_error {
public:
  NCDecryptionException(const char *msg): std::runtime_error(msg) { }
};

class NCConfigurationException: public std::runtime_error {
public:
  NCConfigurationException(const char *msg): std::runtime_error(msg) { }
};

}

#endif // FILE_NC_EXCEPTIONS_HPP_INCLUDED
