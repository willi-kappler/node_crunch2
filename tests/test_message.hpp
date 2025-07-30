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

/*
TEST_CASE("Encode / decode a message", "[message]" ) {
    NCMessageType message_type = NCMessageType::Init;
    NCNodeID node_id = NCNodeID();
    std::string msg1 = "Hello world, this is a test for encoding a message. Add some more content: test, test, test, test, test, test, test, test.";
    std::vector<uint8_t> data(msg1.begin(), msg1.end());

    std::string key1 = "12345678901234567890123456789012";

    auto encoded_message1 = nc_encode_message(message_type, node_id.id, data, key1);
    REQUIRE(encoded_message1.has_value() == true);

    REQUIRE(encoded_message1->data.size() == 187);

    auto decoded_message1 = nc_decode_message(*encoded_message1, key1);
    REQUIRE(decoded_message1.has_value() == true);

    REQUIRE(decoded_message1->data.size() == msg1.size());
    REQUIRE(decoded_message1->msg_type == message_type);

    std::string msg2(decoded_message1->data.begin(), decoded_message1->data.end());
    REQUIRE(msg2 == msg1);
}

TEST_CASE("Encode / decode a message, empty node id", "[message]" ) {
    NCMessageType message_type = NCMessageType::Init;
    std::string node_id = "";
    std::string msg1 = "Hello world, this is a test for encoding a message. Add some more content: test, test, test, test, test, test, test, test.";
    std::vector<uint8_t> data(msg1.begin(), msg1.end());

    std::string key1 = "12345678901234567890123456789012";

    auto encoded_message1 = nc_encode_message(message_type, node_id, data, key1);
    REQUIRE(encoded_message1.has_value() == true);

    REQUIRE(encoded_message1->data.size() == 123);

    auto decoded_message1 = nc_decode_message(*encoded_message1, key1);
    REQUIRE(decoded_message1.has_value() == true);

    REQUIRE(decoded_message1->data.size() == msg1.size());
    REQUIRE(decoded_message1->msg_type == message_type);

    std::string msg2(decoded_message1->data.begin(), decoded_message1->data.end());
    REQUIRE(msg2 == msg1);
}

TEST_CASE("Encode / decode a message, empty node id, empty data", "[message]" ) {
    NCMessageType message_type = NCMessageType::Init;
    std::string node_id = "";
    std::vector<uint8_t> data = std::vector<uint8_t>();

    std::string key1 = "12345678901234567890123456789012";

    auto encoded_message1 = nc_encode_message(message_type, node_id, data, key1);
    REQUIRE(encoded_message1.has_value() == true);

    REQUIRE(encoded_message1->data.size() == 35);

    auto decoded_message1 = nc_decode_message(*encoded_message1, key1);
    REQUIRE(decoded_message1.has_value() == true);

    REQUIRE(decoded_message1->data.size() == 0);
    REQUIRE(decoded_message1->msg_type == message_type);
}

TEST_CASE("Generate heartbeat message", "[message]" ) {
    std::string key = "12345678901234567890123456789012";
    NCNodeID node_id = NCNodeID();

    auto message1 = nc_gen_heartbeat_message(node_id, key);
    REQUIRE(message1.has_value() == true);

    auto message2 = nc_decode_message(*message1, key);
    REQUIRE(message2.has_value() == true);

    REQUIRE(message2->msg_type == NCMessageType::Heartbeat);
    REQUIRE(message2->node_id.id == node_id.id);
    REQUIRE(message2->data.size() == 0);
}

TEST_CASE("Generate heartbeat ok message", "[message]" ) {
    std::string key = "12345678901234567890123456789012";
    NCNodeID node_id = NCNodeID();

    auto message1 = nc_gen_heartbeat_message_ok(key);
    REQUIRE(message1.has_value() == true);

    auto message2 = nc_decode_message(*message1, key);
    REQUIRE(message2.has_value() == true);

    REQUIRE(message2->msg_type == NCMessageType::HeartbeatOK);
    REQUIRE(message2->node_id.id != node_id.id);
    REQUIRE(message2->data.size() == 0);
}

TEST_CASE("Generate heartbeat error message", "[message]" ) {
    std::string key = "12345678901234567890123456789012";
    NCNodeID node_id = NCNodeID();

    auto message1 = nc_gen_heartbeat_message_error(key);
    REQUIRE(message1.has_value() == true);

    auto message2 = nc_decode_message(*message1, key);
    REQUIRE(message2.has_value() == true);

    REQUIRE(message2->msg_type == NCMessageType::HeartbeatError);
    REQUIRE(message2->node_id.id != node_id.id);
    REQUIRE(message2->data.size() == 0);
}

TEST_CASE("Generate init message", "[message]" ) {
    std::string key = "12345678901234567890123456789012";
    NCNodeID node_id = NCNodeID();

    auto message1 = nc_gen_init_message(node_id, key);
    REQUIRE(message1.has_value() == true);

    auto message2 = nc_decode_message(*message1, key);
    REQUIRE(message2.has_value() == true);

    REQUIRE(message2->msg_type == NCMessageType::Init);
    REQUIRE(message2->node_id.id == node_id.id);
    REQUIRE(message2->data.size() == 0);
}

TEST_CASE("Generate init ok message", "[message]" ) {
    std::string key = "12345678901234567890123456789012";
    NCNodeID node_id = NCNodeID();
    std::vector<uint8_t> data = {6, 7, 8, 9};

    auto message1 = nc_gen_init_message_ok(data, key);
    REQUIRE(message1.has_value() == true);

    auto message2 = nc_decode_message(*message1, key);
    REQUIRE(message2.has_value() == true);

    REQUIRE(message2->msg_type == NCMessageType::InitOK);
    REQUIRE(message2->node_id.id != node_id.id);
    REQUIRE(message2->data == data);
}

TEST_CASE("Generate init error message", "[message]" ) {
    std::string key = "12345678901234567890123456789012";
    NCNodeID node_id = NCNodeID();

    auto message1 = nc_gen_init_message_error(key);
    REQUIRE(message1.has_value() == true);

    auto message2 = nc_decode_message(*message1, key);
    REQUIRE(message2.has_value() == true);

    REQUIRE(message2->msg_type == NCMessageType::InitError);
    REQUIRE(message2->node_id.id != node_id.id);
    REQUIRE(message2->data.size() == 0);
}

TEST_CASE("Generate result message", "[message]" ) {
    std::string key = "12345678901234567890123456789012";
    NCNodeID node_id = NCNodeID();
    std::vector<uint8_t> data = {6, 7, 8, 9};

    auto message1 = nc_gen_result_message(node_id, data, key);
    REQUIRE(message1.has_value() == true);

    auto message2 = nc_decode_message(*message1, key);
    REQUIRE(message2.has_value() == true);

    REQUIRE(message2->msg_type == NCMessageType::NewResultFromNode);
    REQUIRE(message2->node_id.id == node_id.id);
    REQUIRE(message2->data == data);
}

TEST_CASE("Generate need more data message", "[message]" ) {
    std::string key = "12345678901234567890123456789012";
    NCNodeID node_id = NCNodeID();

    auto message1 = nc_gen_need_more_data_message(node_id, key);
    REQUIRE(message1.has_value() == true);

    auto message2 = nc_decode_message(*message1, key);
    REQUIRE(message2.has_value() == true);

    REQUIRE(message2->msg_type == NCMessageType::NodeNeedsMoreData);
    REQUIRE(message2->node_id.id == node_id.id);
    REQUIRE(message2->data.size() == 0);
}

TEST_CASE("Generate new data from server message", "[message]" ) {
    std::string key = "12345678901234567890123456789012";
    NCNodeID node_id = NCNodeID();
    std::vector<uint8_t> data = {6, 7, 8, 9};

    auto message1 = nc_gen_new_data_message(data, key);
    REQUIRE(message1.has_value() == true);

    auto message2 = nc_decode_message(*message1, key);
    REQUIRE(message2.has_value() == true);

    REQUIRE(message2->msg_type == NCMessageType::NewDataFromServer);
    REQUIRE(message2->node_id.id != node_id.id);
    REQUIRE(message2->data == data);
}

TEST_CASE("Generate result ok message", "[message]" ) {
    std::string key = "12345678901234567890123456789012";
    NCNodeID node_id = NCNodeID();

    auto message1 = nc_gen_result_ok_message(key);
    REQUIRE(message1.has_value() == true);

    auto message2 = nc_decode_message(*message1, key);
    REQUIRE(message2.has_value() == true);

    REQUIRE(message2->msg_type == NCMessageType::ResultOK);
    REQUIRE(message2->node_id.id != node_id.id);
    REQUIRE(message2->data.size() == 0);
}

TEST_CASE("Generate quit message", "[message]" ) {
    std::string key = "12345678901234567890123456789012";
    NCNodeID node_id = NCNodeID();

    auto message1 = nc_gen_quit_message(key);
    REQUIRE(message1.has_value() == true);

    auto message2 = nc_decode_message(*message1, key);
    REQUIRE(message2.has_value() == true);

    REQUIRE(message2->msg_type == NCMessageType::Quit);
    REQUIRE(message2->node_id.id != node_id.id);
    REQUIRE(message2->data.size() == 0);
}
*/
