/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file contains the tests for the utils functions.

    Run only configuration tests:
    xmake run -w ./ nc_test [util]
*/

// STD includes:

// External includes:
#include <snitch/snitch.hpp>

// Local includes:
#include "nc_util.hpp"

using namespace NodeCrunch2;

TEST_CASE("Convert a number to a vector", "[util]" ) {
    uint32_t num1 = 0xDEADBEEFu;
    std::vector<uint8_t> buff1(4);

    nc_to_big_endian_bytes(num1, buff1);

    REQUIRE(buff1[0] == 0xDEu);
    REQUIRE(buff1[1] == 0xADu);
    REQUIRE(buff1[2] == 0xBEu);
    REQUIRE(buff1[3] == 0xEFu);

    num1 = 0xAABBCCDDu;

    nc_to_big_endian_bytes(num1, buff1);

    REQUIRE(buff1[0] == 0xAAu);
    REQUIRE(buff1[1] == 0xBBu);
    REQUIRE(buff1[2] == 0xCCu);
    REQUIRE(buff1[3] == 0xDDu);
}

TEST_CASE("Convert a number to an array", "[util]" ) {
    uint32_t num1 = 0xDEADBEEFu;
    std::array<uint8_t, 4> buff1;

    nc_to_big_endian_bytes(num1, buff1);

    REQUIRE(buff1[0] == 0xDEu);
    REQUIRE(buff1[1] == 0xADu);
    REQUIRE(buff1[2] == 0xBEu);
    REQUIRE(buff1[3] == 0xEFu);

    num1 = 0xAABBCCDDu;

    nc_to_big_endian_bytes(num1, buff1);

    REQUIRE(buff1[0] == 0xAAu);
    REQUIRE(buff1[1] == 0xBBu);
    REQUIRE(buff1[2] == 0xCCu);
    REQUIRE(buff1[3] == 0xDDu);
}

TEST_CASE("Convert a vector to a number", "[util]" ) {
    std::vector<uint8_t> buff1 = {0xDEu, 0xADu, 0xBEu, 0xEFu};

    uint32_t num1 = nc_from_big_endian_bytes(buff1);

    REQUIRE(num1 == 0xDEADBEEFu);

    buff1 = {0xAAu, 0xBBu, 0xCCu, 0xDDu};

    num1 = nc_from_big_endian_bytes(buff1);

    REQUIRE(num1 == 0xAABBCCDDu);
}

TEST_CASE("Convert a array to a number", "[util]" ) {
    std::array<uint8_t, 4> buff1 = {0xDEu, 0xADu, 0xBEu, 0xEFu};

    uint32_t num1 = nc_from_big_endian_bytes(buff1);

    REQUIRE(num1 == 0xDEADBEEFu);

    buff1 = {0xAAu, 0xBBu, 0xCCu, 0xDDu};

    num1 = nc_from_big_endian_bytes(buff1);

    REQUIRE(num1 == 0xAABBCCDDu);
}
