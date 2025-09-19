/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file contains the tests for the node class.

    Run only configuration tests:
    xmake run -w ./ nc_test [node]
*/

// STD includes:

// External includes:
#include <snitch/snitch.hpp>

// Local includes:
#include "nc_node.hpp"
#include "nc_message.hpp"

using namespace NodeCrunch2;

const std::string TEST_KEY("12345678901234567890123456789012");

class TestDataProcessor: public NCNodeDataProcessor {
    public:
        void nc_init(std::vector<uint8_t> data) override;
        [[nodiscard]] std::vector<uint8_t> nc_process_data(std::vector<uint8_t> data) override;
};

void TestDataProcessor::nc_init([[maybe_unused]] std::vector<uint8_t> data) {
}

[[nodiscard]] std::vector<uint8_t> TestDataProcessor::nc_process_data([[maybe_unused]] std::vector<uint8_t> data) {
    std::vector<uint8_t> result;

    return result;
}

class TestSocket: public NCNetworkSocketBase {
    public:
        // API
        void nc_send_data(std::vector<uint8_t> const data) override;
        [[nodiscard]] std::vector<uint8_t> nc_receive_data() override;
        [[nodiscard]] std::string nc_address() override;

        // Constructor
        TestSocket();

        // Members used in test cases
        std::vector<uint8_t> buffer_intern;
        NCMessageCodecServer message_codec_intern;
        std::vector<NCNodeID> node_ids;
};

TestSocket::TestSocket():
    buffer_intern(),
    message_codec_intern(TEST_KEY),
    node_ids()
    {}

void TestSocket::nc_send_data(std::vector<uint8_t> const data) {
    // TODO: process data and react:

    NCDecodedMessageFromNode node_message = message_codec_intern.nc_decode_message_from_node(NCEncodedMessageToServer(data));
    NCNodeID const node_id = node_message.node_id;
    NCEncodedMessageToNode msg_to_node;

    node_ids.push_back(node_id);

    switch (node_message.msg_type) {
        case NCNodeMessageType::Init:
            msg_to_node = message_codec_intern.nc_gen_unknown_error();
        break;
        case NCNodeMessageType::Heartbeat:
            msg_to_node = message_codec_intern.nc_gen_unknown_error();
        break;
        case NCNodeMessageType::NodeNeedsMoreData:
            msg_to_node = message_codec_intern.nc_gen_unknown_error();
        break;
        case NCNodeMessageType::NewResultFromNode:
            msg_to_node = message_codec_intern.nc_gen_unknown_error();
        break;
        default:
            msg_to_node = message_codec_intern.nc_gen_unknown_error();
    }

    buffer_intern = msg_to_node.data;
}

[[nodiscard]] std::vector<uint8_t> TestSocket::nc_receive_data() {
    return buffer_intern;
}

[[nodiscard]] std::string TestSocket::nc_address() {
    return std::string("TestSocket");
}


class TestClient: public NCNetworkClientBase {
    public:
        NCNetworkSocketBase nc_connect() override;
};

NCNetworkSocketBase TestClient::nc_connect() {
    return TestSocket();
}



TEST_CASE("Create node, send init message", "[node]" ) {
    NCConfiguration config1 = NCConfiguration(TEST_KEY);
    TestDataProcessor data_processor1;
    TestClient client1;
    NCNode node1(config1, data_processor1, client1);
}
