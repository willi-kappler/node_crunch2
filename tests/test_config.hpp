/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file contains the tests for the configuration class.

    Run only configuration tests:
    xmake run -w ./ nc_test [configuration]
*/

// External includes:
#include <snitch/snitch.hpp>

// Local includes:
#include "nc_config.hpp"
#include "nc_exceptions.hpp"

TEST_CASE("Create valid default configuration", "[configuration]" ) {
    NCConfiguration config1 = NCConfiguration("12345678901234567890123456789012");

    REQUIRE(config1.server_address == "127.0.0.1");
    REQUIRE(config1.server_port == 3100);
    REQUIRE(config1.heartbeat_timeout == 60*5);
    REQUIRE(config1.quit_counter == 10);
    REQUIRE(config1.secret_key == "12345678901234567890123456789012");
}

TEST_CASE("Create invalid default configuration", "[configuration]" ) {
    REQUIRE_THROWS_AS(NCConfiguration("12345"), NCInvalidKeyException);
}

TEST_CASE("Read valid JSON configuration", "[configuration]" ) {
    std::expected<NCConfiguration, NCConfigurationError> config1 = nc_config_from_file("tests/config1.json");

    REQUIRE(config1.has_value() == true);
    REQUIRE(config1->server_address == "33.44.55.66");
    REQUIRE(config1->server_port == 9999);
    REQUIRE(config1->heartbeat_timeout == 10);
    REQUIRE(config1->quit_counter == 3);
    REQUIRE(config1->secret_key == "123456789012345678901234567890A1");
}

TEST_CASE("File open error", "[configuration]") {
    std::expected<NCConfiguration, NCConfigurationError> config1 = nc_config_from_file("does_not_exist.json");

    REQUIRE(config1.has_value() == false);
    REQUIRE(config1.error() == NCConfigurationError::NCFileOpenError);
}

TEST_CASE("Only server address", "[configuration]") {
    std::string input1{R"({"secret_key": "123456789012345678901234567890A2", "server_address": "11.22.33.44"})"};
    std::expected<NCConfiguration, NCConfigurationError> config1 = nc_config_from_string(input1);

    REQUIRE(config1.has_value() == true);
    REQUIRE(config1->server_address == "11.22.33.44");
    REQUIRE(config1->server_port == 3100);
    REQUIRE(config1->heartbeat_timeout == 60*5);
    REQUIRE(config1->quit_counter == 10);
    REQUIRE(config1->secret_key == "123456789012345678901234567890A2");
}

TEST_CASE("Only server port", "[configuration]") {
    std::string input1{R"({"secret_key": "123456789012345678901234567890A3", "server_port": 1111})"};
    std::expected<NCConfiguration, NCConfigurationError> config1 = nc_config_from_string(input1);

    REQUIRE(config1.has_value() == true);
    REQUIRE(config1->server_address == "127.0.0.1");
    REQUIRE(config1->server_port == 1111);
    REQUIRE(config1->heartbeat_timeout == 60*5);
    REQUIRE(config1->quit_counter == 10);
    REQUIRE(config1->secret_key == "123456789012345678901234567890A3");
}

TEST_CASE("Only heartbeat timeout", "[configuration]") {
    std::string input1{R"({"secret_key": "123456789012345678901234567890A4", "heartbeat_timeout": 500})"};
    std::expected<NCConfiguration, NCConfigurationError> config1 = nc_config_from_string(input1);

    REQUIRE(config1.has_value() == true);
    REQUIRE(config1->server_address == "127.0.0.1");
    REQUIRE(config1->server_port == 3100);
    REQUIRE(config1->heartbeat_timeout == 500);
    REQUIRE(config1->quit_counter == 10);
    REQUIRE(config1->secret_key == "123456789012345678901234567890A4");
}

TEST_CASE("Only quit counter", "[configuration]") {
    std::string input1{R"({"secret_key": "123456789012345678901234567890A5", "quit_counter": 20})"};
    std::expected<NCConfiguration, NCConfigurationError> config1 = nc_config_from_string(input1);

    REQUIRE(config1.has_value() == true);
    REQUIRE(config1->server_address == "127.0.0.1");
    REQUIRE(config1->server_port == 3100);
    REQUIRE(config1->heartbeat_timeout == 60*5);
    REQUIRE(config1->quit_counter == 20);
    REQUIRE(config1->secret_key == "123456789012345678901234567890A5");
}

TEST_CASE("Missing secret key", "[configuration]") {
    std::string input1{R"({})"};
    std::expected<NCConfiguration, NCConfigurationError> config1 = nc_config_from_string(input1);

    REQUIRE(config1.has_value() == false);
    REQUIRE(config1.error() == NCConfigurationError::NCMissingSecretKey);
}

TEST_CASE("Invalid server port", "[configuration]") {
    std::string input1{R"({"secret_key": "123456789012345678901234567890A6", "server_port": 0})"};
    std::expected<NCConfiguration, NCConfigurationError> config1 = nc_config_from_string(input1);

    REQUIRE(config1.has_value() == false);
    REQUIRE(config1.error() == NCConfigurationError::NCInvalidPort);
}

TEST_CASE("Invalid heartbeat timeout", "[configuration]") {
    std::string input1{R"({"secret_key": "123456789012345678901234567890A7", "heartbeat_timeout": 0})"};
    std::expected<NCConfiguration, NCConfigurationError> config1 = nc_config_from_string(input1);

    REQUIRE(config1.has_value() == false);
    REQUIRE(config1.error() == NCConfigurationError::NCInvalidHeartbeat);
}
