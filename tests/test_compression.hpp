/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file contains the tests for the message encoding / decoding functions.

    Run only configuration tests:
    xmake run -w ./ nc_test [compression]
*/

// STD includes:
#include <bit>

// External includes:
#include <snitch/snitch.hpp>

// Local includes:
#include "nc_compression.hpp"

TEST_CASE("Convert a number to a vector", "[compression]" ) {
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

TEST_CASE("Convert a vector to a number", "[compression]" ) {
    std::vector<uint8_t> buff1 = {0xDEu, 0xADu, 0xBEu, 0xEFu};

    uint32_t num1 = nc_from_big_endian_bytes(buff1);

    REQUIRE(num1 == 0xDEADBEEFu);

    buff1 = {0xAAu, 0xBBu, 0xCCu, 0xDDu};

    num1 = nc_from_big_endian_bytes(buff1);

    REQUIRE(num1 == 0xAABBCCDDu);
}

TEST_CASE("Compress / decompress a message", "[compression]" ) {
    std::string msg1 = "Hello world, this is a test for compressing a message. Add some more content: test, test, test, test, test, test, test, test.";
    NCDecompressedMessage msg1r;
    msg1r.data.assign(msg1.begin(), msg1.end());

    std::expected<NCCompressedMessage, NCMessageError> compressed_message1 = nc_compress_message(msg1r);
    REQUIRE(compressed_message1.has_value() == true);

    REQUIRE(compressed_message1->data.size() == 99);

    uint32_t msg_size = nc_from_big_endian_bytes(compressed_message1->data);
    REQUIRE(msg_size == msg1.size());

    std::expected<NCDecompressedMessage, NCMessageError> decompressed_message1 = nc_decompress_message(NCCompressedMessage(compressed_message1->data));
    REQUIRE(decompressed_message1.has_value() == true);

    REQUIRE(decompressed_message1->data.size() == msg1.size());

    std::string msg2(decompressed_message1->data.begin(), decompressed_message1->data.end());
    REQUIRE(msg2 == msg1);
}
