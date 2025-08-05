/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file defines the server class
*/

// STD includes:
#include <thread>
#include <chrono>

// External includes:
#include <spdlog/spdlog.h>
#include <asio.hpp>

// Local includes:
#include "nc_network.hpp"
#include "nc_server.hpp"

using asio::ip::tcp;
