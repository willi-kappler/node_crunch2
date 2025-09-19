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

using namespace NodeCrunch2;

TEST_CASE("Encode / decode a message to the server", "[message]" ) {
    NCNodeMessageType const message_type = NCNodeMessageType::Init;
    NCNodeID const node_id = NCNodeID();
    std::string const msg1 = "Hello world, this is a test for encoding a message. Add some more content: test, test, test, test, test, test, test, test.";
    std::vector<uint8_t> const data(msg1.begin(), msg1.end());
    std::string const key1 = "12345678901234567890123456789012";
    NCMessageCodecNode node_codec(key1);
    NCMessageCodecServer server_codec(key1);

    auto const encoded_message1 = node_codec.nc_encode_message_to_server(message_type, data, node_id);
    REQUIRE(encoded_message1.data.size() == 186);

    auto const decoded_message1 = server_codec.nc_decode_message_from_node(encoded_message1);
    REQUIRE(decoded_message1.data.size() == msg1.size());
    REQUIRE(decoded_message1.msg_type == message_type);
    REQUIRE(decoded_message1.node_id.id == node_id.id);

    std::string const msg2(decoded_message1.data.begin(), decoded_message1.data.end());
    REQUIRE(msg2 == msg1);
}

TEST_CASE("Encode / decode an empty message to the server", "[message]" ) {
    NCNodeMessageType const message_type = NCNodeMessageType::Init;
    NCNodeID const node_id = NCNodeID();
    std::vector<uint8_t> const data;
    std::string const key1 = "12345678901234567890123456789012";
    NCMessageCodecNode node_codec(key1);
    NCMessageCodecServer server_codec(key1);

    auto const encoded_message1 = node_codec.nc_encode_message_to_server(message_type, data, node_id);
    REQUIRE(encoded_message1.data.size() == 99);

    auto const decoded_message1 = server_codec.nc_decode_message_from_node(encoded_message1);
    REQUIRE(decoded_message1.data.size() == 0);
    REQUIRE(decoded_message1.msg_type == message_type);
    REQUIRE(decoded_message1.node_id.id == node_id.id);
}

TEST_CASE("Encode / decode a message to the node", "[message]" ) {
    NCNodeMessageType const message_type = NCNodeMessageType::Heartbeat;
    NCNodeID const node_id = NCNodeID();
    std::string const msg1 = "Hello world, this is a test for encoding a message. Add some more content: test, test, test, test, test, test, test, test.";
    std::vector<uint8_t> const data(msg1.begin(), msg1.end());
    std::string const key1 = "12345678901234567890123456789012";
    NCMessageCodecNode node_codec(key1);
    NCMessageCodecServer server_codec(key1);

    auto const encoded_message1 = node_codec.nc_encode_message_to_server(message_type, data, node_id);
    REQUIRE(encoded_message1.data.size() == 186);

    auto const decoded_message1 = server_codec.nc_decode_message_from_node(encoded_message1);
    REQUIRE(decoded_message1.data.size() == msg1.size());
    REQUIRE(decoded_message1.msg_type == message_type);

    std::string const msg2(decoded_message1.data.begin(), decoded_message1.data.end());
    REQUIRE(msg2 == msg1);
}

TEST_CASE("Encode / decode an empty message to the node", "[message]" ) {
    NCNodeMessageType const message_type = NCNodeMessageType::NewResultFromNode;
    NCNodeID const node_id = NCNodeID();
    std::vector<uint8_t> const data;
    std::string const key1 = "12345678901234567890123456789012";
    NCMessageCodecNode node_codec(key1);
    NCMessageCodecServer server_codec(key1);

    auto const encoded_message1 = node_codec.nc_encode_message_to_server(message_type, data, node_id);
    REQUIRE(encoded_message1.data.size() == 99);

    auto const decoded_message1 = server_codec.nc_decode_message_from_node(encoded_message1);
    REQUIRE(decoded_message1.data.size() == 0);
    REQUIRE(decoded_message1.msg_type == message_type);
}

TEST_CASE("Generate heartbeat message", "[message]" ) {
    std::string const key = "12345678901234567890123456789012";
    NCNodeID const node_id = NCNodeID();
    NCMessageCodecNode node_codec(key);
    NCMessageCodecServer server_codec(key);

    auto const message1 = node_codec.nc_gen_heartbeat_message(node_id);
    auto const message2 = server_codec.nc_decode_message_from_node(message1);

    REQUIRE(message2.msg_type == NCNodeMessageType::Heartbeat);
    REQUIRE(message2.node_id.id == node_id.id);
    REQUIRE(message2.data.size() == 0);
}

TEST_CASE("Generate heartbeat ok message", "[message]" ) {
    std::string const key = "12345678901234567890123456789012";
    NCNodeID const node_id = NCNodeID();
    NCMessageCodecNode node_codec(key);
    NCMessageCodecServer server_codec(key);

    auto const message1 = server_codec.nc_gen_heartbeat_message_ok();
    auto const message2 = node_codec.nc_decode_message_from_server(message1);

    REQUIRE(message2.msg_type == NCServerMessageType::HeartbeatOK);
    REQUIRE(message2.data.size() == 0);
}

TEST_CASE("Generate init message", "[message]" ) {
    std::string const key = "12345678901234567890123456789012";
    NCNodeID const node_id = NCNodeID();
    NCMessageCodecNode node_codec(key);
    NCMessageCodecServer server_codec(key);

    auto const message1 = node_codec.nc_gen_init_message(node_id);
    auto const message2 = server_codec.nc_decode_message_from_node(message1);

    REQUIRE(message2.msg_type == NCNodeMessageType::Init);
    REQUIRE(message2.node_id.id == node_id.id);
    REQUIRE(message2.data.size() == 0);
}

TEST_CASE("Generate init ok message", "[message]" ) {
    std::string const key = "12345678901234567890123456789012";
    std::vector<uint8_t> const data = {6, 7, 8, 9};
    NCNodeID const node_id = NCNodeID();
    NCMessageCodecNode node_codec(key);
    NCMessageCodecServer server_codec(key);

    auto const message1 = server_codec.nc_gen_init_message_ok(data);
    auto const message2 = node_codec.nc_decode_message_from_server(message1);

    REQUIRE(message2.msg_type == NCServerMessageType::InitOK);
    REQUIRE(message2.data == data);
}

TEST_CASE("Generate result message", "[message]" ) {
    std::string const key = "12345678901234567890123456789012";
    NCNodeID const node_id = NCNodeID();
    std::vector<uint8_t> const data = {6, 7, 8, 9};
    NCMessageCodecNode node_codec(key);
    NCMessageCodecServer server_codec(key);

    auto const message1 = node_codec.nc_gen_result_message(data, node_id);
    auto const message2 = server_codec.nc_decode_message_from_node(message1);

    REQUIRE(message2.msg_type == NCNodeMessageType::NewResultFromNode);
    REQUIRE(message2.node_id.id == node_id.id);
    REQUIRE(message2.data == data);
}

TEST_CASE("Generate need more data message", "[message]" ) {
    std::string const key = "12345678901234567890123456789012";
    NCNodeID const node_id = NCNodeID();
    NCMessageCodecNode node_codec(key);
    NCMessageCodecServer server_codec(key);

    auto const message1 = node_codec.nc_gen_need_more_data_message(node_id);
    auto const message2 = server_codec.nc_decode_message_from_node(message1);

    REQUIRE(message2.msg_type == NCNodeMessageType::NodeNeedsMoreData);
    REQUIRE(message2.node_id.id == node_id.id);
    REQUIRE(message2.data.size() == 0);
}

TEST_CASE("Generate new data from server message", "[message]" ) {
    std::string const key = "12345678901234567890123456789012";
    std::vector<uint8_t> const data = {6, 7, 8, 9};
    NCNodeID const node_id = NCNodeID();
    NCMessageCodecNode node_codec(key);
    NCMessageCodecServer server_codec(key);

    auto const message1 = server_codec.nc_gen_new_data_message(data);
    auto const message2 = node_codec.nc_decode_message_from_server(message1);

    REQUIRE(message2.msg_type == NCServerMessageType::NewDataFromServer);
    REQUIRE(message2.data == data);
}

TEST_CASE("Generate result ok message", "[message]" ) {
    std::string const key = "12345678901234567890123456789012";
    NCNodeID const node_id = NCNodeID();
    NCMessageCodecNode node_codec(key);
    NCMessageCodecServer server_codec(key);

    auto const message1 = server_codec.nc_gen_result_ok_message();
    auto const message2 = node_codec.nc_decode_message_from_server(message1);

    REQUIRE(message2.msg_type == NCServerMessageType::ResultOK);
    REQUIRE(message2.data.size() == 0);
}

TEST_CASE("Generate quit message", "[message]" ) {
    std::string const key = "12345678901234567890123456789012";
    NCNodeID const node_id = NCNodeID();
    NCMessageCodecNode node_codec(key);
    NCMessageCodecServer server_codec(key);

    auto const message1 = server_codec.nc_gen_quit_message();
    auto const message2 = node_codec.nc_decode_message_from_server(message1);

    REQUIRE(message2.msg_type == NCServerMessageType::Quit);
    REQUIRE(message2.data.size() == 0);
}

TEST_CASE("Generate invalid node id message", "[message]" ) {
    std::string const key = "12345678901234567890123456789012";
    NCNodeID const node_id = NCNodeID();
    NCMessageCodecNode node_codec(key);
    NCMessageCodecServer server_codec(key);

    auto const message1 = server_codec.nc_gen_invalid_node_id_error();
    auto const message2 = node_codec.nc_decode_message_from_server(message1);

    REQUIRE(message2.msg_type == NCServerMessageType::InvalidNodeID);
    REQUIRE(message2.data.size() == 0);
}

TEST_CASE("Generate unknown error message", "[message]" ) {
    std::string const key = "12345678901234567890123456789012";
    NCNodeID const node_id = NCNodeID();
    NCMessageCodecNode node_codec(key);
    NCMessageCodecServer server_codec(key);

    auto const message1 = server_codec.nc_gen_unknown_error();
    auto const message2 = node_codec.nc_decode_message_from_server(message1);

    REQUIRE(message2.msg_type == NCServerMessageType::UnknownError);
    REQUIRE(message2.data.size() == 0);
}
