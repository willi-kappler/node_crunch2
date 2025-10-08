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

using namespace NodeCrunch2;

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
        bool job_done;
        uint8_t save_data_called;
        std::vector<NCNodeID> timeout_nodes;
        std::vector<NCNodeID> data_nodes;
        std::vector<NCNodeID> process_nodes;
};

TestServerDataProcessor::TestServerDataProcessor(std::vector<uint8_t> data):
    NCServerDataProcessor(),
    initial_data(data),
    new_data(data),
    job_done(false),
    save_data_called(0),
    timeout_nodes(),
    data_nodes(),
    process_nodes()
    {}

[[nodiscard]] std::vector<uint8_t> TestServerDataProcessor::nc_get_init_data() {
    return initial_data;
}

[[nodiscard]] bool TestServerDataProcessor::nc_is_job_done() {
    return job_done;
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
    auto const sleep_time = std::chrono::seconds(2);
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
                case 10:
                    data_intern->msg_to_server = data_intern->message_codec.nc_gen_need_more_data_message(data_intern->node_id);
                break;
                case 20:
                    data_intern->msg_to_server = data_intern->message_codec.nc_gen_heartbeat_message(data_intern->node_id);
                break;
                case 30:
                    data_intern->msg_to_server = data_intern->message_codec.nc_gen_heartbeat_message(new_node_id);
                break;
                default:
                    data_intern->msg_to_server = data_intern->message_codec.nc_encode_message_to_server(invalid_message, {}, new_node_id);
            }
        break;
        case NCServerMessageType::NewDataFromServer:
            spdlog::info("NewDataFromServer");

            size_t i;
            for (i = 0; i < server_message.data.size(); i++) {
                data_intern->node_data[i] = server_message.data[i] + 1;
            }

            data_intern->msg_to_server = data_intern->message_codec.nc_gen_result_message(data_intern->node_data, data_intern->node_id);
            // Simulate long computation:
            std::this_thread::sleep_for(sleep_time);
        break;
        case NCServerMessageType::ResultOK:
            spdlog::info("ResultOK");
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

class TestServer: public NCNetworkServerBase {
    public:
        std::unique_ptr<NCNetworkSocketBase> nc_accept() override;
        TestServer(std::shared_ptr<TestServerSocketData> init_data);

        std::shared_ptr<TestServerSocketData> data_intern;
};

TestServer::TestServer(std::shared_ptr<TestServerSocketData> init_data):
    NCNetworkServerBase(),
    data_intern(init_data)
    {}

std::unique_ptr<NCNetworkSocketBase> TestServer::nc_accept() {
    return std::make_unique<TestServerSocket>(data_intern);
}

TEST_CASE("Create server, answer init message", "[server]" ) {
    NCConfiguration config1 = NCConfiguration("12345678901234567890123456789012");
    //NCServer server1(config1);
}

TEST_CASE("Create node, send init message (test mode 10)", "[node]" ) {
    NCConfiguration config1 = NCConfiguration(TEST_SERVER_KEY);
    NCNodeID node_id;
    std::shared_ptr<TestServerSocketData> init_data = std::make_shared<TestServerSocketData>(node_id, 10);
    std::vector<uint8_t> first_data = {1, 2, 3, 4, 5};
    std::unique_ptr<TestServerDataProcessor> data_processor1 = std::make_unique<TestServerDataProcessor>(first_data);
    data_processor1->job_done = true;

    std::unique_ptr<NCNetworkServerBase> network_server1 = std::make_unique<TestServer>(init_data);
    NCServer server1(config1, std::move(data_processor1), std::move(network_server1));
    server1.nc_run();
}
