/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file contains the tests for the message encoding / decoding functions.

    Run only configuration tests:
    xmake run -w ./ nc_test [message]
*/

// STD includes:
#include <bit>

// External includes:
#include <snitch/snitch.hpp>

// Local includes:
#include "nc_message.hpp"

TEST_CASE("Encode / decode a message", "[message]" ) {
    std::string msg1 = "Hello world, this is a test for encoding a message. Add some more content: test, test, test, test, test, test, test, test.";
    std::vector<uint8_t> msg1v(msg1.begin(), msg1.end());

    std::string key1 = "12345678901234567890123456789012";

    std::expected<NCEncodedMessage, NCMessageError> encoded_message1 = nc_encode_message(NCRawMessage(msg1v), key1);
    REQUIRE(encoded_message1.has_value() == true);

    REQUIRE(encoded_message1->data.size() == 93);

    std::expected<NCDecodedMessage, NCMessageError> decoded_message1 = nc_decode_message(*encoded_message1, key1);
    REQUIRE(decoded_message1.has_value() == true);

    REQUIRE(decoded_message1->data.size() == 122);

    std::string msg2(decoded_message1->data.begin(), decoded_message1->data.end());
    REQUIRE(msg2 == msg1);
}
