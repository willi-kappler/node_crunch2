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

        TestDataProcessor();
};

TestDataProcessor::TestDataProcessor():
    NCNodeDataProcessor()
    {}

void TestDataProcessor::nc_init([[maybe_unused]] std::vector<uint8_t> data) {
}

[[nodiscard]] std::vector<uint8_t> TestDataProcessor::nc_process_data([[maybe_unused]] std::vector<uint8_t> data) {
    return std::vector<uint8_t>();
}

class TestSocketData {
    public:

        // Members used in test cases
        std::vector<uint8_t> server_data;
        NCEncodedMessageToNode msg_to_node;
        NCMessageCodecServer message_codec;
        uint8_t heartbeat_counter;
        std::vector<NCNodeID> node_ids;
        std::vector<NCNodeMessageType> node_messages;
        uint8_t test_mode;

        TestSocketData();
};

TestSocketData::TestSocketData():
    server_data(),
    msg_to_node(),
    message_codec(TEST_KEY),
    heartbeat_counter(),
    node_ids(),
    node_messages(),
    test_mode()
    {}

class TestSocket: public NCNetworkSocketBase {
    public:
        // API
        void nc_send_data(std::vector<uint8_t> const data) override;
        [[nodiscard]] std::vector<uint8_t> nc_receive_data() override;
        [[nodiscard]] std::string nc_address() override;

        // Constructor
        TestSocket(std::shared_ptr<TestSocketData> init_data);

        std::shared_ptr<TestSocketData> data_intern;
};

TestSocket::TestSocket(std::shared_ptr<TestSocketData> init_data):
    NCNetworkSocketBase(),
    data_intern(init_data)
    {}

void TestSocket::nc_send_data(std::vector<uint8_t> const data) {
    // TODO: process data and react:

    NCDecodedMessageFromNode node_message = data_intern->message_codec.nc_decode_message_from_node(NCEncodedMessageToServer(data));
    NCNodeID const node_id = node_message.node_id;

    data_intern->node_ids.push_back(node_id);
    data_intern->node_messages.push_back(node_message.msg_type);

    switch (node_message.msg_type) {
        case NCNodeMessageType::Init:
            if (data_intern->test_mode == 10) {
                data_intern->msg_to_node = data_intern->message_codec.nc_gen_quit_message();
            } else {
                data_intern->msg_to_node = data_intern->message_codec.nc_gen_init_message_ok(data_intern->server_data);
            }
        break;
        case NCNodeMessageType::Heartbeat:
            data_intern->heartbeat_counter++;
            if (data_intern->test_mode == 20) {
                data_intern->msg_to_node = data_intern->message_codec.nc_gen_quit_message();
            } else {
                data_intern->msg_to_node = data_intern->message_codec.nc_gen_heartbeat_message_ok();
            }
        break;
        case NCNodeMessageType::NodeNeedsMoreData:
            if (data_intern->test_mode == 30) {
                data_intern->msg_to_node = data_intern->message_codec.nc_gen_quit_message();
            } else {
                data_intern->msg_to_node = data_intern->message_codec.nc_gen_new_data_message(data_intern->server_data);
            }
        break;
        case NCNodeMessageType::NewResultFromNode:
            for (size_t i = 0; i < data_intern->server_data.size(); i++) {
                data_intern->server_data[i] = node_message.data[i] + 10;
            }

            /*
            std::transform(node_message.data.begin(), node_message.data.end(), server_data.begin(),
               [](uint8_t x) { return x + 10; });
            */

            if (data_intern->test_mode == 40) {
                data_intern->msg_to_node = data_intern->message_codec.nc_gen_quit_message();
            } else {
                data_intern->msg_to_node = data_intern->message_codec.nc_gen_result_ok_message();
            }
        break;
        default:
            data_intern->msg_to_node = data_intern->message_codec.nc_gen_unknown_error();
    }
}

[[nodiscard]] std::vector<uint8_t> TestSocket::nc_receive_data() {
    return data_intern->msg_to_node.data;
}

[[nodiscard]] std::string TestSocket::nc_address() {
    return std::string("TestSocket");
}

class TestClient: public NCNetworkClientBase {
    public:
        std::unique_ptr<NCNetworkSocketBase> nc_connect() override;
        TestClient(std::shared_ptr<TestSocketData> init_data);

        std::shared_ptr<TestSocketData> data_intern;
};

TestClient::TestClient(std::shared_ptr<TestSocketData> init_data):
    NCNetworkClientBase(),
    data_intern(init_data)
    {}

std::unique_ptr<NCNetworkSocketBase> TestClient::nc_connect() {
    return std::make_unique<TestSocket>(data_intern);
}

TEST_CASE("Create node, send init message", "[node]" ) {
    NCConfiguration config1 = NCConfiguration(TEST_KEY);
    std::shared_ptr<TestSocketData> init_data = std::make_shared<TestSocketData>();
    init_data->test_mode = 10;

    std::unique_ptr<NCNodeDataProcessor> data_processor1 = std::make_unique<TestDataProcessor>();
    std::unique_ptr<NCNetworkClientBase> client1 = std::make_unique<TestClient>(init_data);
    NCNode node1(config1, std::move(data_processor1), std::move(client1));
    node1.nc_run();

    REQUIRE(init_data->server_data.size() == 0);
    NCEncodedMessageToNode expected_message = init_data->message_codec.nc_gen_quit_message();
    //REQUIRE(init_data->msg_to_node.data == expected_message.data);
    REQUIRE(init_data->heartbeat_counter == 0);
    REQUIRE(init_data->node_ids.size() == 1);
    REQUIRE(init_data->node_messages.size() == 1);
    REQUIRE(init_data->node_messages[0] == NCNodeMessageType::Init);
    REQUIRE(init_data->test_mode == 10);

}
