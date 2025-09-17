/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file contains the tests for the node class.

    Run only configuration tests:
    xmake run -w ./ nc_test [node]
*/

// STD includes:

// External includes:
#include <snitch/snitch.hpp>

// Local includes:
#include "nc_node.hpp"
#include "nc_message.hpp"

using namespace NodeCrunch2;

TEST_CASE("Create node, send init message", "[node]" ) {
    NCConfiguration config1 = NCConfiguration("12345678901234567890123456789012");
    //NCNode node1(config1);
}
