/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file contains the tests for the nodeid class.

    Run only nodeid tests:
    xmake run -w ./ nc_test [nodeid]
*/

// STD includes:
#include <iostream>
#include <unordered_map>

// External includes:
#include <snitch/snitch.hpp>

// Local includes:
#include "nc_nodeid.hpp"

using namespace nodcru2;

TEST_CASE("Create valid nodeid", "[nodeid]" ) {
    NCNodeID nodeid1;

    // std::cout << "NodeId: " << nodeid1.id << std::endl;

    REQUIRE(nodeid1.id.length() == NC_NODEID_LENGTH);
}

TEST_CASE("Compare nodeid", "[nodeid]" ) {
    NCNodeID nodeid1, nodeid2;

    // std::cout << "NodeId1: " << nodeid1.id << std::endl;
    // std::cout << "NodeId2: " << nodeid2.id << std::endl;

    REQUIRE(nodeid1 != nodeid2);
}

TEST_CASE("Hash nodeid", "[nodeid]" ) {
    NCNodeID nodeid1, nodeid2, nodeid3;
    std::unordered_map<NCNodeID, std::string> map1 = {
        {nodeid1, "node 1"},
        {nodeid2, "node 2"},
        {nodeid3, "node 3"}
    };

    REQUIRE(map1[nodeid1] == "node 1");
    REQUIRE(map1[nodeid2] == "node 2");
    REQUIRE(map1[nodeid3] == "node 3");
}
