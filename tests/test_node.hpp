/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file contains the tests for the node class.

    Run only configuration tests:
    xmake run -w ./ nc_test [node]
*/

// STD includes:
#include <thread>

// External includes:
#include <snitch/snitch.hpp>

// Local includes:
#include "nc_node.hpp"
#include "nc_message.hpp"

using namespace NodeCrunch2;

const std::string TEST_NODE_KEY("12345678901234567890123456789012");

class TestNodeDataProcessor: public NCNodeDataProcessor {
    public:
        void nc_init(std::vector<uint8_t> data) override;
        [[nodiscard]] std::vector<uint8_t> nc_process_data(std::vector<uint8_t> data) override;

        TestNodeDataProcessor();

        std::vector<uint8_t> initial_data;
};

TestNodeDataProcessor::TestNodeDataProcessor():
    NCNodeDataProcessor(),
    initial_data()
    {}

void TestNodeDataProcessor::nc_init(std::vector<uint8_t> data) {
    initial_data = data;
}

[[nodiscard]] std::vector<uint8_t> TestNodeDataProcessor::nc_process_data(std::vector<uint8_t> data) {
    std::vector<uint8_t> result;

    size_t i, j = 0;
    uint8_t value = 0;

    for (i = 0; i < data.size(); i++) {
        value = data[i] * 2;
        value = initial_data[j] + value;
        result.push_back(value);

        j++;
        if (j >= initial_data.size()) {
            j = 0;
        }
    }

    // Simulate long computation:
    auto const sleep_time = std::chrono::seconds(2);
    std::this_thread::sleep_for(sleep_time);

    return result;
}

class TestNodeSocketData {
    public:

        // Members used in test cases
        std::vector<uint8_t> server_data;
        NCEncodedMessageToNode msg_to_node;
        NCMessageCodecServer message_codec;
        uint8_t heartbeat_counter;
        std::vector<NCNodeID> node_ids;
        std::vector<NCNodeMessageType> node_messages;
        uint8_t test_mode;

        TestNodeSocketData();
};

TestNodeSocketData::TestNodeSocketData():
    server_data(),
    msg_to_node(),
    message_codec(TEST_NODE_KEY),
    heartbeat_counter(),
    node_ids(),
    node_messages(),
    test_mode()
    {}

class TestNodeSocket: public NCNetworkSocketBase {
    public:
        // API
        void nc_send_data(std::vector<uint8_t> const data) override;
        [[nodiscard]] std::vector<uint8_t> nc_receive_data() override;
        [[nodiscard]] std::string nc_address() override;

        // Constructor
        TestNodeSocket(std::shared_ptr<TestNodeSocketData> init_data);

        std::shared_ptr<TestNodeSocketData> data_intern;
};

TestNodeSocket::TestNodeSocket(std::shared_ptr<TestNodeSocketData> init_data):
    NCNetworkSocketBase(),
    data_intern(init_data)
    {}

void TestNodeSocket::nc_send_data(std::vector<uint8_t> const data) {
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
        case NCNodeMessageType::NewResultFromNode:
            data_intern->server_data.clear();

            for (uint8_t v: node_message.data) {
                data_intern->server_data.push_back(v);
            }

            if (data_intern->test_mode == 30) {
                data_intern->msg_to_node = data_intern->message_codec.nc_gen_quit_message();
            } else {
                data_intern->msg_to_node = data_intern->message_codec.nc_gen_result_ok_message();
            }
        break;
        case NCNodeMessageType::NodeNeedsMoreData:
            if (data_intern->test_mode == 40) {
                data_intern->msg_to_node = data_intern->message_codec.nc_gen_quit_message();
            } else {
                data_intern->msg_to_node = data_intern->message_codec.nc_gen_new_data_message(data_intern->server_data);
            }
        break;
        default:
            data_intern->msg_to_node = data_intern->message_codec.nc_gen_unknown_error();
    }
}

[[nodiscard]] std::vector<uint8_t> TestNodeSocket::nc_receive_data() {
    return data_intern->msg_to_node.data;
}

[[nodiscard]] std::string TestNodeSocket::nc_address() {
    return std::string("TestNodeSocket");
}

class TestClient: public NCNetworkClientBase {
    public:
        std::unique_ptr<NCNetworkSocketBase> nc_connect() override;
        TestClient(std::shared_ptr<TestNodeSocketData> init_data);

        std::shared_ptr<TestNodeSocketData> data_intern;
};

TestClient::TestClient(std::shared_ptr<TestNodeSocketData> init_data):
    NCNetworkClientBase(),
    data_intern(init_data)
    {}

std::unique_ptr<NCNetworkSocketBase> TestClient::nc_connect() {
    return std::make_unique<TestNodeSocket>(data_intern);
}

TEST_CASE("Create node, send init message (test mode 10)", "[node]" ) {
    NCConfiguration config1 = NCConfiguration(TEST_NODE_KEY);
    std::shared_ptr<TestNodeSocketData> init_data = std::make_shared<TestNodeSocketData>();
    init_data->test_mode = 10;
    init_data->server_data = {1, 2, 3, 4, 5};

    std::unique_ptr<TestNodeDataProcessor> data_processor1 = std::make_unique<TestNodeDataProcessor>();
    std::unique_ptr<TestClient> client1 = std::make_unique<TestClient>(init_data);
    NCNode node1(config1, std::move(data_processor1), std::move(client1));
    node1.nc_run();

    REQUIRE(init_data->server_data.size() == 5);
    REQUIRE(init_data->server_data[0] == 1);
    REQUIRE(init_data->server_data[1] == 2);
    REQUIRE(init_data->server_data[2] == 3);
    REQUIRE(init_data->server_data[3] == 4);
    REQUIRE(init_data->server_data[4] == 5);
    NCEncodedMessageToNode expected_message = init_data->message_codec.nc_gen_quit_message();
    // REQUIRE(init_data->msg_to_node.data == expected_message.data);
    REQUIRE(init_data->heartbeat_counter == 0);
    REQUIRE(init_data->node_ids.size() == 1);
    REQUIRE(init_data->node_messages.size() == 1);
    REQUIRE(init_data->node_messages[0] == NCNodeMessageType::Init);
    REQUIRE(init_data->test_mode == 10);

}

TEST_CASE("Create node, send heartbeat message (test mode 20)", "[node]" ) {
    NCConfiguration config1 = NCConfiguration(TEST_NODE_KEY);
    config1.heartbeat_timeout = 10;
    std::shared_ptr<TestNodeSocketData> init_data = std::make_shared<TestNodeSocketData>();
    init_data->test_mode = 20;
    init_data->server_data = {1, 2, 3, 4, 5};

    std::unique_ptr<TestNodeDataProcessor> data_processor1 = std::make_unique<TestNodeDataProcessor>();
    std::unique_ptr<TestClient> client1 = std::make_unique<TestClient>(init_data);
    NCNode node1(config1, std::move(data_processor1), std::move(client1));
    node1.nc_run();

    REQUIRE(init_data->server_data.size() == 5);
    REQUIRE(init_data->server_data[0] == 31);
    REQUIRE(init_data->server_data[1] == 62);
    REQUIRE(init_data->server_data[2] == 93);
    REQUIRE(init_data->server_data[3] == 124);
    REQUIRE(init_data->server_data[4] == 155);

    NCEncodedMessageToNode expected_message = init_data->message_codec.nc_gen_quit_message();
    // REQUIRE(init_data->msg_to_node.data == expected_message.data);
    REQUIRE(init_data->heartbeat_counter == 1);
    REQUIRE(init_data->node_ids.size() == 11);

    REQUIRE(init_data->node_messages.size() == 11);
    REQUIRE(init_data->node_messages[0] == NCNodeMessageType::Init);
    REQUIRE(init_data->node_messages[1] == NCNodeMessageType::NodeNeedsMoreData);
    REQUIRE(init_data->node_messages[2] == NCNodeMessageType::NewResultFromNode);
    REQUIRE(init_data->node_messages[3] == NCNodeMessageType::NodeNeedsMoreData);
    REQUIRE(init_data->node_messages[4] == NCNodeMessageType::NewResultFromNode);
    REQUIRE(init_data->node_messages[5] == NCNodeMessageType::NodeNeedsMoreData);
    REQUIRE(init_data->node_messages[6] == NCNodeMessageType::NewResultFromNode);
    REQUIRE(init_data->node_messages[7] == NCNodeMessageType::NodeNeedsMoreData);
    REQUIRE(init_data->node_messages[8] == NCNodeMessageType::NewResultFromNode);
    REQUIRE(init_data->node_messages[9] == NCNodeMessageType::NodeNeedsMoreData);
    REQUIRE(init_data->node_messages[10] == NCNodeMessageType::Heartbeat);

    REQUIRE(init_data->test_mode == 20);

}

TEST_CASE("Create node, send new result message (test mode 30)", "[node]" ) {
    NCConfiguration config1 = NCConfiguration(TEST_NODE_KEY);
    config1.heartbeat_timeout = 10;
    std::shared_ptr<TestNodeSocketData> init_data = std::make_shared<TestNodeSocketData>();
    init_data->test_mode = 30;
    init_data->server_data = {1, 2, 3, 4, 5};

    std::unique_ptr<TestNodeDataProcessor> data_processor1 = std::make_unique<TestNodeDataProcessor>();
    std::unique_ptr<TestClient> client1 = std::make_unique<TestClient>(init_data);
    NCNode node1(config1, std::move(data_processor1), std::move(client1));
    node1.nc_run();

    REQUIRE(init_data->server_data.size() == 5);
    REQUIRE(init_data->server_data[0] == 3);
    REQUIRE(init_data->server_data[1] == 6);
    REQUIRE(init_data->server_data[2] == 9);
    REQUIRE(init_data->server_data[3] == 12);
    REQUIRE(init_data->server_data[4] == 15);

    NCEncodedMessageToNode expected_message = init_data->message_codec.nc_gen_quit_message();
    // REQUIRE(init_data->msg_to_node.data == expected_message.data);
    REQUIRE(init_data->heartbeat_counter == 1);
    REQUIRE(init_data->node_ids.size() == 4);

    REQUIRE(init_data->node_messages.size() == 4);
    REQUIRE(init_data->node_messages[0] == NCNodeMessageType::Init);
    REQUIRE(init_data->node_messages[1] == NCNodeMessageType::NodeNeedsMoreData);
    REQUIRE(init_data->node_messages[2] == NCNodeMessageType::NewResultFromNode);
    REQUIRE(init_data->node_messages[3] == NCNodeMessageType::Heartbeat);

    REQUIRE(init_data->test_mode == 30);

}

TEST_CASE("Create node, send node needs data message (test mode 40)", "[node]" ) {
    NCConfiguration config1 = NCConfiguration(TEST_NODE_KEY);
    config1.heartbeat_timeout = 10;
    std::shared_ptr<TestNodeSocketData> init_data = std::make_shared<TestNodeSocketData>();
    init_data->test_mode = 40;
    init_data->server_data = {1, 2, 3, 4, 5};

    std::unique_ptr<TestNodeDataProcessor> data_processor1 = std::make_unique<TestNodeDataProcessor>();
    std::unique_ptr<TestClient> client1 = std::make_unique<TestClient>(init_data);
    NCNode node1(config1, std::move(data_processor1), std::move(client1));
    node1.nc_run();

    REQUIRE(init_data->server_data.size() == 5);
    REQUIRE(init_data->server_data[0] == 1);
    REQUIRE(init_data->server_data[1] == 2);
    REQUIRE(init_data->server_data[2] == 3);
    REQUIRE(init_data->server_data[3] == 4);
    REQUIRE(init_data->server_data[4] == 5);

    NCEncodedMessageToNode expected_message = init_data->message_codec.nc_gen_quit_message();
    // REQUIRE(init_data->msg_to_node.data == expected_message.data);
    // REQUIRE(init_data->heartbeat_counter == 0);
    REQUIRE(init_data->node_ids.size() == 2);

    REQUIRE(init_data->node_messages.size() == 2);
    REQUIRE(init_data->node_messages[0] == NCNodeMessageType::Init);
    REQUIRE(init_data->node_messages[1] == NCNodeMessageType::NodeNeedsMoreData);

    REQUIRE(init_data->test_mode == 40);

}
