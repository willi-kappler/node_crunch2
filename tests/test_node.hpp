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
        TestSocket(std::vector<uint8_t> init_data);

        // Members used in test cases
        std::vector<uint8_t> server_data;
        NCEncodedMessageToNode msg_to_node;;
        NCMessageCodecServer message_codec;
        uint8_t heartbeat_counter;
        std::vector<NCNodeID> node_ids;
        std::vector<NCNodeMessageType> node_messages;
        uint8_t test_mode;
};

TestSocket::TestSocket(std::vector<uint8_t> init_data):
    server_data(init_data),
    msg_to_node(),
    message_codec(TEST_KEY),
    heartbeat_counter(0),
    node_ids(),
    node_messages(),
    test_mode(0)
{
    if (init_data.size() == 0) {
        throw std::invalid_argument("init_data is empty!");
    } else {
        test_mode = init_data[0];
    }
}

void TestSocket::nc_send_data(std::vector<uint8_t> const data) {
    // TODO: process data and react:

    NCDecodedMessageFromNode node_message = message_codec.nc_decode_message_from_node(NCEncodedMessageToServer(data));
    NCNodeID const node_id = node_message.node_id;

    node_ids.push_back(node_id);
    node_messages.push_back(node_message.msg_type);

    switch (node_message.msg_type) {
        case NCNodeMessageType::Init:
            if (test_mode == 10) {
                msg_to_node = message_codec.nc_gen_quit_message();
            } else {
                msg_to_node = message_codec.nc_gen_init_message_ok(server_data);
            }
        break;
        case NCNodeMessageType::Heartbeat:
            heartbeat_counter++;
            if (test_mode == 20) {
                msg_to_node = message_codec.nc_gen_quit_message();
            } else {
                msg_to_node = message_codec.nc_gen_heartbeat_message_ok();
            }
        break;
        case NCNodeMessageType::NodeNeedsMoreData:
            if (test_mode == 30) {
                msg_to_node = message_codec.nc_gen_quit_message();
            } else {
                msg_to_node = message_codec.nc_gen_new_data_message(server_data);
            }
        break;
        case NCNodeMessageType::NewResultFromNode:
            for (size_t i = 0; i < server_data.size(); i++) {
                server_data[i] = node_message.data[i] + 10;
            }

            /*
            std::transform(node_message.data.begin(), node_message.data.end(), server_data.begin(),
               [](uint8_t x) { return x + 10; });
            */

            if (test_mode == 40) {
                msg_to_node = message_codec.nc_gen_quit_message();
            } else {
                msg_to_node = message_codec.nc_gen_result_ok_message();
            }
        break;
        default:
            msg_to_node = message_codec.nc_gen_unknown_error();
    }
}

[[nodiscard]] std::vector<uint8_t> TestSocket::nc_receive_data() {
    return msg_to_node.data;
}

[[nodiscard]] std::string TestSocket::nc_address() {
    return std::string("TestSocket");
}

class TestClient: public NCNetworkClientBase {
    public:
        NCNetworkSocketBase nc_connect() override;
        TestClient(std::vector<uint8_t> init_data);

        TestSocket test_socket;
};

TestClient::TestClient(std::vector<uint8_t> init_data):
    test_socket(init_data)
{
    if (init_data.size() == 0) {
        throw std::invalid_argument("init_data is empty!");
    }
}

NCNetworkSocketBase TestClient::nc_connect() {
    return test_socket;
}

TEST_CASE("Create node, send init message", "[node]" ) {
    NCConfiguration config1 = NCConfiguration(TEST_KEY);
    TestDataProcessor data_processor1;
    TestClient client1({10});
    NCNode node1(config1, data_processor1, client1);
    node1.nc_run();
}
