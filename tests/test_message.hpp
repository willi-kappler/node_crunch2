/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file contains the tests for the message encoding / decoding functions.

    Run only configuration tests:
    xmake run -w ./ nc_test [message]
*/

// STD includes:
#include <iostream>

// External includes:
#include <snitch/snitch.hpp>

// Local includes:
#include "nc_message.hpp"

TEST_CASE("Encode / decode a message", "[message]" ) {
    NCMessageType message_type = NCMessageType::Init;
    std::string node_id = "";
    std::string msg1 = "Hello world, this is a test for encoding a message. Add some more content: test, test, test, test, test, test, test, test.";
    std::vector<uint8_t> data(msg1.begin(), msg1.end());

    std::string key1 = "12345678901234567890123456789012";

    std::expected<NCEncodedMessage, NCMessageError> encoded_message1 = nc_encode_message(message_type, node_id, data, key1);
    REQUIRE(encoded_message1.has_value() == true);

    REQUIRE(encoded_message1->data.size() == 256);

    std::expected<NCDecodedMessage, NCMessageError> decoded_message1 = nc_decode_message(*encoded_message1, key1);
    std::cout << "Error: " << static_cast<uint32_t>(decoded_message1.error()) << std::endl;
    REQUIRE(decoded_message1.has_value() == true);

    REQUIRE(decoded_message1->data.size() == 122);
    REQUIRE(decoded_message1->msg_type == message_type);

    std::string msg2(decoded_message1->data.begin(), decoded_message1->data.end());
    REQUIRE(msg2 == msg1);
}
