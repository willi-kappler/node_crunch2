/*
    Node Crunch2
    SPDX-License-Identifier: MIT
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file contains the tests for the message encoding / decoding functions.

    Run only configuration tests:
    builddir/node_crunch2_tests [message]
*/

// STD includes:
#include <iostream>

// External includes:
#include <snitch/snitch.hpp>

// Local includes:
#include "nodcru2/nc_message.hpp"

using namespace nodcru2;

TEST_CASE("Encode / decode a message to the server", "[message]" ) {
    const NCNodeMessageType message_type = NCNodeMessageType::Init;
    const NCNodeID node_id = NCNodeID();
    const std::string msg1 = "Hello world, this is a test for encoding a message. Add some more content: test, test, test, test, test, test, test, test.";
    const std::vector<uint8_t> data(msg1.begin(), msg1.end());
    const std::string key1 = "12345678901234567890123456789012";
    NCMessageCodecNode node_codec(key1);
    NCMessageCodecServer server_codec(key1);

    const auto encoded_message1 = node_codec.nc_encode_message_to_server(message_type, data, node_id);
    REQUIRE(encoded_message1.data.size() == static_cast<size_t>(186));

    const auto decoded_message1 = server_codec.nc_decode_message_from_node(encoded_message1);
    REQUIRE(decoded_message1.data.size() == msg1.size());
    REQUIRE(decoded_message1.msg_type == message_type);
    REQUIRE(decoded_message1.node_id.id == node_id.id);

    const std::string msg2(decoded_message1.data.begin(), decoded_message1.data.end());
    REQUIRE(msg2 == msg1);
}

TEST_CASE("Encode / decode an empty message to the server", "[message]" ) {
    const NCNodeMessageType message_type = NCNodeMessageType::Init;
    const NCNodeID node_id = NCNodeID();
    const std::vector<uint8_t> data;
    const std::string key1 = "12345678901234567890123456789012";
    NCMessageCodecNode node_codec(key1);
    NCMessageCodecServer server_codec(key1);

    const auto encoded_message1 = node_codec.nc_encode_message_to_server(message_type, data, node_id);
    REQUIRE(encoded_message1.data.size() == static_cast<size_t>(99));

    const auto decoded_message1 = server_codec.nc_decode_message_from_node(encoded_message1);
    REQUIRE(decoded_message1.data.size() == static_cast<size_t>(0));
    REQUIRE(decoded_message1.msg_type == message_type);
    REQUIRE(decoded_message1.node_id.id == node_id.id);
}

TEST_CASE("Encode / decode a message to the node", "[message]" ) {
    const NCNodeMessageType message_type = NCNodeMessageType::Heartbeat;
    const NCNodeID node_id = NCNodeID();
    const std::string msg1 = "Hello world, this is a test for encoding a message. Add some more content: test, test, test, test, test, test, test, test.";
    const std::vector<uint8_t> data(msg1.begin(), msg1.end());
    const std::string key1 = "12345678901234567890123456789012";
    NCMessageCodecNode node_codec(key1);
    NCMessageCodecServer server_codec(key1);

    const auto encoded_message1 = node_codec.nc_encode_message_to_server(message_type, data, node_id);
    REQUIRE(encoded_message1.data.size() == static_cast<size_t>(186));

    const auto decoded_message1 = server_codec.nc_decode_message_from_node(encoded_message1);
    REQUIRE(decoded_message1.data.size() == msg1.size());
    REQUIRE(decoded_message1.msg_type == message_type);

    const std::string msg2(decoded_message1.data.begin(), decoded_message1.data.end());
    REQUIRE(msg2 == msg1);
}

TEST_CASE("Encode / decode an empty message to the node", "[message]" ) {
    const NCNodeMessageType message_type = NCNodeMessageType::NewResultFromNode;
    const NCNodeID node_id = NCNodeID();
    const std::vector<uint8_t> data;
    const std::string key1 = "12345678901234567890123456789012";
    NCMessageCodecNode node_codec(key1);
    NCMessageCodecServer server_codec(key1);

    const auto encoded_message1 = node_codec.nc_encode_message_to_server(message_type, data, node_id);
    REQUIRE(encoded_message1.data.size() == static_cast<size_t>(99));

    const auto decoded_message1 = server_codec.nc_decode_message_from_node(encoded_message1);
    REQUIRE(decoded_message1.data.size() == static_cast<size_t>(0));
    REQUIRE(decoded_message1.msg_type == message_type);
}

TEST_CASE("Generate heartbeat message", "[message]" ) {
    const std::string key = "12345678901234567890123456789012";
    const NCNodeID node_id = NCNodeID();
    NCMessageCodecNode node_codec(key);
    NCMessageCodecServer server_codec(key);

    const auto message1 = node_codec.nc_gen_heartbeat_message(node_id);
    const auto message2 = server_codec.nc_decode_message_from_node(message1);

    REQUIRE(message2.msg_type == NCNodeMessageType::Heartbeat);
    REQUIRE(message2.node_id.id == node_id.id);
    REQUIRE(message2.data.size() == static_cast<size_t>(0));
}

TEST_CASE("Generate heartbeat ok message", "[message]" ) {
    const std::string key = "12345678901234567890123456789012";
    const NCNodeID node_id = NCNodeID();
    NCMessageCodecNode node_codec(key);
    NCMessageCodecServer server_codec(key);

    const auto message1 = server_codec.nc_gen_heartbeat_message_ok();
    const auto message2 = node_codec.nc_decode_message_from_server(message1);

    REQUIRE(message2.msg_type == NCServerMessageType::HeartbeatOK);
    REQUIRE(message2.data.size() == static_cast<size_t>(0));
}

TEST_CASE("Generate init message", "[message]" ) {
    const std::string key = "12345678901234567890123456789012";
    const NCNodeID node_id = NCNodeID();
    NCMessageCodecNode node_codec(key);
    NCMessageCodecServer server_codec(key);

    const auto message1 = node_codec.nc_gen_init_message(node_id);
    const auto message2 = server_codec.nc_decode_message_from_node(message1);

    REQUIRE(message2.msg_type == NCNodeMessageType::Init);
    REQUIRE(message2.node_id.id == node_id.id);
    REQUIRE(message2.data.size() == static_cast<size_t>(0));
}

TEST_CASE("Generate init ok message", "[message]" ) {
    const std::string key = "12345678901234567890123456789012";
    const std::vector<uint8_t> data = {6, 7, 8, 9};
    const NCNodeID node_id = NCNodeID();
    NCMessageCodecNode node_codec(key);
    NCMessageCodecServer server_codec(key);

    const auto message1 = server_codec.nc_gen_init_message_ok(data);
    const auto message2 = node_codec.nc_decode_message_from_server(message1);

    REQUIRE(message2.msg_type == NCServerMessageType::InitOK);
    REQUIRE(message2.data == data);
}

TEST_CASE("Generate result message", "[message]" ) {
    const std::string key = "12345678901234567890123456789012";
    const NCNodeID node_id = NCNodeID();
    const std::vector<uint8_t> data = {6, 7, 8, 9};
    NCMessageCodecNode node_codec(key);
    NCMessageCodecServer server_codec(key);

    const auto message1 = node_codec.nc_gen_result_message(data, node_id);
    const auto message2 = server_codec.nc_decode_message_from_node(message1);

    REQUIRE(message2.msg_type == NCNodeMessageType::NewResultFromNode);
    REQUIRE(message2.node_id.id == node_id.id);
    REQUIRE(message2.data == data);
}

TEST_CASE("Generate need more data message", "[message]" ) {
    const std::string key = "12345678901234567890123456789012";
    const NCNodeID node_id = NCNodeID();
    NCMessageCodecNode node_codec(key);
    NCMessageCodecServer server_codec(key);

    const auto message1 = node_codec.nc_gen_need_more_data_message(node_id);
    const auto message2 = server_codec.nc_decode_message_from_node(message1);

    REQUIRE(message2.msg_type == NCNodeMessageType::NodeNeedsMoreData);
    REQUIRE(message2.node_id.id == node_id.id);
    REQUIRE(message2.data.size() == static_cast<size_t>(0));
}

TEST_CASE("Generate new data from server message", "[message]" ) {
    const std::string key = "12345678901234567890123456789012";
    const std::vector<uint8_t> data = {6, 7, 8, 9};
    const NCNodeID node_id = NCNodeID();
    NCMessageCodecNode node_codec(key);
    NCMessageCodecServer server_codec(key);

    const auto message1 = server_codec.nc_gen_new_data_message(data);
    const auto message2 = node_codec.nc_decode_message_from_server(message1);

    REQUIRE(message2.msg_type == NCServerMessageType::NewDataFromServer);
    REQUIRE(message2.data == data);
}

TEST_CASE("Generate result ok message", "[message]" ) {
    const std::string key = "12345678901234567890123456789012";
    const NCNodeID node_id = NCNodeID();
    NCMessageCodecNode node_codec(key);
    NCMessageCodecServer server_codec(key);

    const auto message1 = server_codec.nc_gen_result_ok_message();
    const auto message2 = node_codec.nc_decode_message_from_server(message1);

    REQUIRE(message2.msg_type == NCServerMessageType::ResultOK);
    REQUIRE(message2.data.size() == static_cast<size_t>(0));
}

TEST_CASE("Generate quit message", "[message]" ) {
    const std::string key = "12345678901234567890123456789012";
    const NCNodeID node_id = NCNodeID();
    NCMessageCodecNode node_codec(key);
    NCMessageCodecServer server_codec(key);

    const auto message1 = server_codec.nc_gen_quit_message();
    const auto message2 = node_codec.nc_decode_message_from_server(message1);

    REQUIRE(message2.msg_type == NCServerMessageType::Quit);
    REQUIRE(message2.data.size() == static_cast<size_t>(0));
}

TEST_CASE("Generate invalid node id message", "[message]" ) {
    const std::string key = "12345678901234567890123456789012";
    const NCNodeID node_id = NCNodeID();
    NCMessageCodecNode node_codec(key);
    NCMessageCodecServer server_codec(key);

    const auto message1 = server_codec.nc_gen_invalid_node_id_error();
    const auto message2 = node_codec.nc_decode_message_from_server(message1);

    REQUIRE(message2.msg_type == NCServerMessageType::InvalidNodeID);
    REQUIRE(message2.data.size() == static_cast<size_t>(0));
}

TEST_CASE("Generate unknown error message", "[message]" ) {
    const std::string key = "12345678901234567890123456789012";
    const NCNodeID node_id = NCNodeID();
    NCMessageCodecNode node_codec(key);
    NCMessageCodecServer server_codec(key);

    const auto message1 = server_codec.nc_gen_unknown_error();
    const auto message2 = node_codec.nc_decode_message_from_server(message1);

    REQUIRE(message2.msg_type == NCServerMessageType::UnknownError);
    REQUIRE(message2.data.size() == static_cast<size_t>(0));
}
