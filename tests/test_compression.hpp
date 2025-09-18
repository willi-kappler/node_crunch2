/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file contains the tests for the message encoding / decoding functions.

    Run only configuration tests:
    xmake run -w ./ nc_test [compression]
*/

// STD includes:
// #include <bit>

// External includes:
#include <snitch/snitch.hpp>

// Local includes:
#include "nc_compression.hpp"
#include "nc_util.hpp"

using namespace NodeCrunch2;

TEST_CASE("Compress / decompress a message", "[compression]" ) {
    NCCompressor compressor;
    std::string msg1 = "Hello world, this is a test for compressing a message. Add some more content: test, test, test, test, test, test, test, test.";
    NCDecompressedMessage msg1r;
    msg1r.data.assign(msg1.begin(), msg1.end());

    auto compressed_message1 = compressor.nc_compress_message(msg1r);

    REQUIRE(compressed_message1.data.size() == 99);

    uint32_t msg_size = nc_from_big_endian_bytes(compressed_message1.data);
    REQUIRE(msg_size == msg1.size());

    auto decompressed_message1 = compressor.nc_decompress_message(NCCompressedMessage(compressed_message1.data));

    REQUIRE(decompressed_message1.data.size() == msg1.size());

    std::string msg2(decompressed_message1.data.begin(), decompressed_message1.data.end());
    REQUIRE(msg2 == msg1);
}

TEST_CASE("NonCompressor", "[compression]" ) {
    NCNonCompressor non_compressor;
    std::string msg1 = "Hello world, this is a test for compressing a message. Add some more content: test, test, test, test, test, test, test, test.";
    NCDecompressedMessage msg1r;
    msg1r.data.assign(msg1.begin(), msg1.end());

    auto compressed_message1 = non_compressor.nc_compress_message(msg1r);

    REQUIRE(compressed_message1.data.size() - 4 == msg1r.data.size());

    uint32_t msg_size = nc_from_big_endian_bytes(compressed_message1.data);
    REQUIRE(msg_size == msg1.size());

    auto decompressed_message1 = non_compressor.nc_decompress_message(NCCompressedMessage(compressed_message1.data));

    REQUIRE(decompressed_message1.data.size() == msg1.size());

    std::string msg2(decompressed_message1.data.begin(), decompressed_message1.data.end());
    REQUIRE(msg2 == msg1);
}
