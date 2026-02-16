/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file contains the tests for the node and server class.

    Run only configuration tests:
    xmake run -w ./ nc_test [server_node]
*/

// STD includes:

// External includes:
#include <snitch/snitch.hpp>

// Local includes:
#include "nodcru2/nc_server.hpp"
#include "nodcru2/nc_node.hpp"
#include "nodcru2/nc_message.hpp"

using namespace nodcru2;

TEST_CASE("Create node and server, test init message", "[server]" ) {
    NCConfiguration config1 = NCConfiguration("12345678901234567890123456789012");
    //NCServer server1(config1);
}
