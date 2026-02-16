/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file contains the tests for the server class.

    Run only configuration tests:
    xmake run -w ./ nc_test [server]
*/

// STD includes:
// #include <thread>

// External includes:
#include <snitch/snitch.hpp>
#include <spdlog/spdlog.h>

// Local includes:
#include "nc_server.hpp"
#include "nc_message.hpp"

using namespace nodcru2;

const std::string TEST_SERVER_KEY("12345678901234567890123456789012");

class TestServerDataProcessor: public NCServerDataProcessor {
    public:
        [[nodiscard]] std::vector<uint8_t> nc_get_init_data() override;
        [[nodiscard]] bool nc_is_job_done() override;
        void nc_save_data() override;
        void nc_node_timeout(NCNodeID node_id) override;
        [[nodiscard]] std::vector<uint8_t> nc_get_new_data(NCNodeID node_id) override;
        void nc_process_result(NCNodeID node_id, std::vector<uint8_t> result) override;

        TestServerDataProcessor(std::vector<uint8_t> data);

        std::vector<uint8_t> initial_data;
        std::vector<uint8_t> new_data;
        uint8_t job_counter;
        uint8_t save_data_called;
        std::vector<NCNodeID> timeout_nodes;
        std::vector<NCNodeID> data_nodes;
        std::vector<NCNodeID> process_nodes;
};

TestServerDataProcessor::TestServerDataProcessor(std::vector<uint8_t> data):
    NCServerDataProcessor(),
    initial_data(data),
    new_data(data),
    job_counter(0),
    save_data_called(0),
    timeout_nodes(),
    data_nodes(),
    process_nodes()
    {}

[[nodiscard]] std::vector<uint8_t> TestServerDataProcessor::nc_get_init_data() {
    return initial_data;
}

[[nodiscard]] bool TestServerDataProcessor::nc_is_job_done() {
    job_counter++;
    return job_counter >= 5;
}

void TestServerDataProcessor::nc_save_data() {
    save_data_called++;
}

void TestServerDataProcessor::nc_node_timeout(NCNodeID node_id) {
    timeout_nodes.push_back(node_id);
}

[[nodiscard]] std::vector<uint8_t> TestServerDataProcessor::nc_get_new_data(NCNodeID node_id) {
    data_nodes.push_back(node_id);
    return new_data;
}

void TestServerDataProcessor::nc_process_result(NCNodeID node_id, std::vector<uint8_t> result) {
    process_nodes.push_back(node_id);

    size_t i;

    for (i = 0; i < result.size(); i++) {
        new_data[i] = result[i];
    }
}

class TestServerSocketData {
    public:

        // Members used in test cases
        NCNodeID node_id;
        std::vector<uint8_t> node_data;
        NCEncodedMessageToServer msg_to_server;
        NCMessageCodecNode message_codec;
        std::vector<NCServerMessageType> server_messages;
        uint8_t test_mode;

        TestServerSocketData(NCNodeID id, uint8_t mode);
};

TestServerSocketData::TestServerSocketData(NCNodeID id, uint8_t mode):
    node_id(id),
    node_data(),
    msg_to_server(),
    message_codec(TEST_SERVER_KEY),
    server_messages(),
    test_mode(mode)
    {
        msg_to_server = message_codec.nc_gen_init_message(id);
    }

class TestServerSocket: public NCNetworkSocketBase {
    public:
        // API
        void nc_send_data(std::vector<uint8_t> const data) override;
        [[nodiscard]] std::vector<uint8_t> nc_receive_data() override;
        [[nodiscard]] std::string nc_address() override;

        // Constructor
        TestServerSocket(std::shared_ptr<TestServerSocketData> init_data);

        std::shared_ptr<TestServerSocketData> data_intern;
};

TestServerSocket::TestServerSocket(std::shared_ptr<TestServerSocketData> init_data):
    NCNetworkSocketBase(),
    data_intern(init_data)
    {}

void TestServerSocket::nc_send_data(std::vector<uint8_t> const data) {
    NCDecodedMessageFromServer server_message = data_intern->message_codec.nc_decode_message_from_server(NCEncodedMessageToNode(data));
    data_intern->server_messages.push_back(server_message.msg_type);
    NCNodeID new_node_id;
    NCNodeMessageType invalid_message = static_cast<NCNodeMessageType>(100);

    switch (server_message.msg_type) {
        case NCServerMessageType::UnknownError:
            spdlog::info("UnknownError");
        break;
        case NCServerMessageType::HeartbeatOK:
            spdlog::info("HeartbeatOK");
        break;
        case NCServerMessageType::InitOK:
            spdlog::info("InitOK");

            switch (data_intern->test_mode) {
                case 10: // Node needs more data
                    data_intern->msg_to_server = data_intern->message_codec.nc_gen_need_more_data_message(data_intern->node_id);
                break;
                case 20: // Heartbeat
                    data_intern->msg_to_server = data_intern->message_codec.nc_gen_heartbeat_message(data_intern->node_id);
                break;
                case 30: // Invalid (unknown) node id
                    data_intern->msg_to_server = data_intern->message_codec.nc_gen_heartbeat_message(new_node_id);
                break;
                default: // Invalid message type
                    data_intern->msg_to_server = data_intern->message_codec.nc_encode_message_to_server(invalid_message, {}, new_node_id);
            }
        break;
        case NCServerMessageType::NewDataFromServer:
            spdlog::info("NewDataFromServer");

            data_intern->node_data.clear();

            for (uint8_t v: server_message.data) {
                data_intern->node_data.push_back(v + 1);
            }

            data_intern->msg_to_server = data_intern->message_codec.nc_gen_result_message(data_intern->node_data, data_intern->node_id);
        break;
        case NCServerMessageType::ResultOK:
            spdlog::info("ResultOK");
            data_intern->msg_to_server = data_intern->message_codec.nc_gen_need_more_data_message(data_intern->node_id);
        break;
        case NCServerMessageType::InvalidNodeID:
            spdlog::info("InvalidNodeID");
        break;
        case NCServerMessageType::Quit:
            spdlog::info("Quit");
        break;
        default:
            spdlog::info("Unknown message");
    }
}

[[nodiscard]] std::vector<uint8_t> TestServerSocket::nc_receive_data() {
    return data_intern->msg_to_server.data;
}

[[nodiscard]] std::string TestServerSocket::nc_address() {
    return std::string("TestServerSocket");
}

class TestNetworkServer: public NCNetworkServerBase {
    public:
        std::unique_ptr<NCNetworkSocketBase> nc_accept() override;
        TestNetworkServer(std::shared_ptr<TestServerSocketData> init_data);

        std::shared_ptr<TestServerSocketData> data_intern;
};

TestNetworkServer::TestNetworkServer(std::shared_ptr<TestServerSocketData> init_data):
    NCNetworkServerBase(),
    data_intern(init_data)
    {}

std::unique_ptr<NCNetworkSocketBase> TestNetworkServer::nc_accept() {
    auto const sleep_time = std::chrono::seconds(2);
    // Simulate long computation:
    std::this_thread::sleep_for(sleep_time);

    return std::make_unique<TestServerSocket>(data_intern);
}

TEST_CASE("Create server, send need more data message (test mode 10)", "[server]" ) {
    NCConfiguration config1 = NCConfiguration(TEST_SERVER_KEY);
    config1.heartbeat_timeout = 20;

    NCNodeID node_id;
    std::shared_ptr<TestServerSocketData> init_data = std::make_shared<TestServerSocketData>(node_id, 10);
    std::vector<uint8_t> first_data = {1, 2, 3, 4, 5};
    std::shared_ptr<TestServerDataProcessor> data_processor1 = std::make_shared<TestServerDataProcessor>(first_data);
    std::unique_ptr<TestNetworkServer> network_server1 = std::make_unique<TestNetworkServer>(init_data);
    NCServer server1(config1, data_processor1, std::move(network_server1));
    server1.nc_run();

    REQUIRE(init_data->node_data.size() == 5);
    REQUIRE(init_data->node_data[0] == 3);
    REQUIRE(init_data->node_data[1] == 4);
    REQUIRE(init_data->node_data[2] == 5);
    REQUIRE(init_data->node_data[3] == 6);
    REQUIRE(init_data->node_data[4] == 7);

    REQUIRE(init_data->server_messages.size() == 6);
    REQUIRE(init_data->server_messages[0] == NCServerMessageType::InitOK);
    REQUIRE(init_data->server_messages[1] == NCServerMessageType::NewDataFromServer);
    REQUIRE(init_data->server_messages[2] == NCServerMessageType::ResultOK);
    REQUIRE(init_data->server_messages[3] == NCServerMessageType::NewDataFromServer);
    REQUIRE(init_data->server_messages[4] == NCServerMessageType::Quit);
    REQUIRE(init_data->server_messages[5] == NCServerMessageType::Quit);

    REQUIRE(init_data->test_mode == 10);

    REQUIRE(data_processor1->job_counter == 6);
    REQUIRE(data_processor1->save_data_called == 1);
    REQUIRE(data_processor1->timeout_nodes.size() == 0);
    REQUIRE(data_processor1->data_nodes.size() == 2);
    REQUIRE(data_processor1->data_nodes[0] == node_id);
    REQUIRE(data_processor1->data_nodes[1] == node_id);
    REQUIRE(data_processor1->process_nodes.size() == 1);
    REQUIRE(data_processor1->process_nodes[0] == node_id);
}

TEST_CASE("Create server, send heartbeat message (test mode 20)", "[server]" ) {
    NCConfiguration config1 = NCConfiguration(TEST_SERVER_KEY);
    config1.heartbeat_timeout = 20;

    NCNodeID node_id;
    std::shared_ptr<TestServerSocketData> init_data = std::make_shared<TestServerSocketData>(node_id, 20);
    std::vector<uint8_t> first_data = {1, 2, 3, 4, 5};
    std::shared_ptr<TestServerDataProcessor> data_processor1 = std::make_shared<TestServerDataProcessor>(first_data);
    std::unique_ptr<TestNetworkServer> network_server1 = std::make_unique<TestNetworkServer>(init_data);
    NCServer server1(config1, data_processor1, std::move(network_server1));
    server1.nc_run();

    REQUIRE(init_data->node_data.size() == 0);

    REQUIRE(init_data->server_messages.size() == 6);
    REQUIRE(init_data->server_messages[0] == NCServerMessageType::InitOK);
    REQUIRE(init_data->server_messages[1] == NCServerMessageType::HeartbeatOK);
    REQUIRE(init_data->server_messages[2] == NCServerMessageType::HeartbeatOK);
    REQUIRE(init_data->server_messages[3] == NCServerMessageType::HeartbeatOK);
    REQUIRE(init_data->server_messages[4] == NCServerMessageType::Quit);
    REQUIRE(init_data->server_messages[5] == NCServerMessageType::Quit);

    REQUIRE(init_data->test_mode == 20);

    REQUIRE(data_processor1->job_counter == 6);
    REQUIRE(data_processor1->save_data_called == 1);
    REQUIRE(data_processor1->timeout_nodes.size() == 0);
    REQUIRE(data_processor1->data_nodes.size() == 0);
    REQUIRE(data_processor1->process_nodes.size() == 0);
}

TEST_CASE("Create server, send invalid node id (test mode 30)", "[server]" ) {
    NCConfiguration config1 = NCConfiguration(TEST_SERVER_KEY);
    config1.heartbeat_timeout = 20;

    NCNodeID node_id;
    std::shared_ptr<TestServerSocketData> init_data = std::make_shared<TestServerSocketData>(node_id, 30);
    std::vector<uint8_t> first_data = {1, 2, 3, 4, 5};
    std::shared_ptr<TestServerDataProcessor> data_processor1 = std::make_shared<TestServerDataProcessor>(first_data);
    std::unique_ptr<TestNetworkServer> network_server1 = std::make_unique<TestNetworkServer>(init_data);
    NCServer server1(config1, data_processor1, std::move(network_server1));
    server1.nc_run();

    REQUIRE(init_data->node_data.size() == 0);

    REQUIRE(init_data->server_messages.size() == 6);
    REQUIRE(init_data->server_messages[0] == NCServerMessageType::InitOK);
    REQUIRE(init_data->server_messages[1] == NCServerMessageType::InvalidNodeID);
    REQUIRE(init_data->server_messages[2] == NCServerMessageType::InvalidNodeID);
    REQUIRE(init_data->server_messages[3] == NCServerMessageType::InvalidNodeID);
    REQUIRE(init_data->server_messages[4] == NCServerMessageType::Quit);
    REQUIRE(init_data->server_messages[5] == NCServerMessageType::Quit);

    REQUIRE(init_data->test_mode == 30);

    REQUIRE(data_processor1->job_counter == 6);
    REQUIRE(data_processor1->save_data_called == 1);
    REQUIRE(data_processor1->timeout_nodes.size() == 0);
    REQUIRE(data_processor1->data_nodes.size() == 0);
    REQUIRE(data_processor1->process_nodes.size() == 0);
}

TEST_CASE("Create server, send invalid message (test mode 40)", "[server]" ) {
    NCConfiguration config1 = NCConfiguration(TEST_SERVER_KEY);
    config1.heartbeat_timeout = 20;

    NCNodeID node_id;
    std::shared_ptr<TestServerSocketData> init_data = std::make_shared<TestServerSocketData>(node_id, 40);
    std::vector<uint8_t> first_data = {1, 2, 3, 4, 5};
    std::shared_ptr<TestServerDataProcessor> data_processor1 = std::make_shared<TestServerDataProcessor>(first_data);
    std::unique_ptr<TestNetworkServer> network_server1 = std::make_unique<TestNetworkServer>(init_data);
    NCServer server1(config1, data_processor1, std::move(network_server1));
    server1.nc_run();

    REQUIRE(init_data->node_data.size() == 0);

    REQUIRE(init_data->server_messages.size() == 6);
    REQUIRE(init_data->server_messages[0] == NCServerMessageType::InitOK);
    REQUIRE(init_data->server_messages[1] == NCServerMessageType::UnknownError);
    REQUIRE(init_data->server_messages[2] == NCServerMessageType::UnknownError);
    REQUIRE(init_data->server_messages[3] == NCServerMessageType::UnknownError);
    REQUIRE(init_data->server_messages[4] == NCServerMessageType::Quit);
    REQUIRE(init_data->server_messages[5] == NCServerMessageType::Quit);

    REQUIRE(init_data->test_mode == 40);

    REQUIRE(data_processor1->job_counter == 6);
    REQUIRE(data_processor1->save_data_called == 1);
    REQUIRE(data_processor1->timeout_nodes.size() == 0);
    REQUIRE(data_processor1->data_nodes.size() == 0);
    REQUIRE(data_processor1->process_nodes.size() == 0);
}
