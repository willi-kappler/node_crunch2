/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines some helper functions for networking.
*/

// External includes:
#include <spdlog/spdlog.h>

// Local includes:
#include "nc_network.hpp"

[[nodiscard]] NCMessageError nc_send_data(std::vector<uint8_t> const data, tcp::socket& socket) {
    uint32_t data_size = static_cast<uint32_t>(data.size());
    std::array<uint8_t, 4> size_bytes;
    nc_to_big_endian_bytes(data_size, size_bytes);

    asio::error_code asio_error;

    asio::write(socket, asio::buffer(size_bytes), asio_error);
    if (asio_error) {
        spdlog::error("Error while writing data size: {}", asio_error.message());
        return NCMessageError::NetworkWriteError;
    }

    asio::write(socket, asio::buffer(data), asio_error);

    if (asio_error) {
        spdlog::error("Error while writing data: {}", asio_error.message());
        return NCMessageError::NetworkWriteError;
    }

    return NCMessageError::NoError;
}

[[nodiscard]] std::expected<std::vector<uint8_t>, NCMessageError> nc_receive_data(tcp::socket& socket) {
    std::array<uint8_t, 4> size_bytes;
    asio::error_code asio_error;

    asio::read(socket, asio::buffer(size_bytes), asio_error);
    if (asio_error) {
        if (asio_error == asio::error::eof) {
            spdlog::error("Error while reading data size: EOF.");
            return std::unexpected(NCMessageError::NetworkReadError);
        } else {
            spdlog::error("Error while reading data size: {}", asio_error.message());
            return std::unexpected(NCMessageError::NetworkReadError);
        }
    }

    uint32_t data_size = nc_from_big_endian_bytes(size_bytes);

    std::vector<uint8_t> result(data_size);
    if (data_size > 0) {
        asio::read(socket, asio::buffer(result), asio_error);
        if (asio_error) {
            spdlog::error("Error while reading data: {}", asio_error.message());
            return std::unexpected(NCMessageError::NetworkReadError);
        }
    }

    return result;
}
