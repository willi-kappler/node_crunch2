/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines all the exceptions that are used in this code base.
*/

#ifndef FILE_NC_EXCEPTIONS_HPP_INCLUDED
#define FILE_NC_EXCEPTIONS_HPP_INCLUDED

#include <stdexcept>

class NCInvalidKeyException : public std::invalid_argument {
public:
  NCInvalidKeyException() : std::invalid_argument("Size of secret key must be 32 bytes.") { }
};

#endif // FILE_NC_EXCEPTIONS_HPP_INCLUDED
