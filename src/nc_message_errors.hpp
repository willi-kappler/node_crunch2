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
    NoError = 0,
    CipherContextError,
    EncryptInitError,
    DecryptInitError,
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
    SizeMissmatch,
    NetworkConnectError,
    NetworkWriteError,
    NetworkReadError
};

/*
std::ostream& operator<<(std::ostream& os, const NCMessageError& msg_error) {
    switch (msg_error) {
        case NCMessageError::NoError:
            os << "NoError";
            break;
        case NCMessageError::CipherContextError:
            os << "CipherContextError";
            break;
        case NCMessageError::EncryptInitError:
            os << "EncryptInitError";
            break;
        case NCMessageError::DecryptInitError:
            os << "DecryptInitError";
            break;
        case NCMessageError::CipherControllError:
            os << "CipherControllError";
            break;
        case NCMessageError::CreateNonceError:
            os << "CreateNonceError";
            break;
        case NCMessageError::SetNonceError:
            os << "SetNonceError";
            break;
        case NCMessageError::EncryptUpdateError:
            os << "EncryptUpdateError";
            break;
        case NCMessageError::DecryptUpdateError:
            os << "DecryptUpdateError";
            break;
        case NCMessageError::EncryptFinalError:
            os << "EncryptFinalError";
            break;
        case NCMessageError::DecryptFinalError:
            os << "DecryptFinalError";
            break;
        case NCMessageError::CipherGetTagError:
            os << "CipherGetTagError";
            break;
        case NCMessageError::CipherSetTagError:
            os << "CipherSetTagError";
            break;
        case NCMessageError::CompressionError:
            os << "CompressionError";
            break;
        case NCMessageError::DecompressionError:
            os << "DecompressionError";
            break;
        case NCMessageError::EncryptionError:
            os << "EncryptionError";
            break;
        case NCMessageError::DecryptionError:
            os << "DecryptionError";
            break;
        case NCMessageError::SizeMissmatch:
            os << "SizeMissmatch";
            break;
        case NCMessageError::NetworkConnectError:
            os << "NetworkConnectError";
            break;
        case NCMessageError::NetworkWriteError:
            os << "NetworkWriteError";
            break;
        case NCMessageError::NetworkReadError:
            os << "NetworkReadError";
            break;
        default:
            os << "Unknown Error";
            break;
    }
    return os;
}
*/

#endif // FILE_NC_MESSAGE_ERRORS_HPP_INCLUDED