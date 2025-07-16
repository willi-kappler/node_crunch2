/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file contains the tests for the configuration class.

    Run with: xmake run test
*/

// External includes:
#include <snitch/snitch.hpp>

// Local includes:
#include "nc_config.hpp"
#include "nc_exceptions.hpp"

TEST_CASE("Create valid default configuration", "[configuration]" ) {
    NCConfiguration config1 = NCConfiguration("12345678901234567890123456789012");
    REQUIRE( config1.server_address == "127.0.0.1");
    REQUIRE( config1.server_port == 3100);
    REQUIRE( config1.heartbeat_timeout == 60*5);
    REQUIRE( config1.quit_counter == 10);
    REQUIRE( config1.secret_key == "12345678901234567890123456789012");
}

TEST_CASE("Create invalid default configuration", "[configuration]" ) {
    REQUIRE_THROWS_AS(NCConfiguration("12345"), NCInvalidKeyException);
}
