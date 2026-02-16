/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file contains the tests for the message encrypting / decrypting functions.

    Run only configuration tests:
    xmake run -w ./ nc_test [message]
*/

// External includes:
#include <snitch/snitch.hpp>

// Local includes:
#include "nodcru2/nc_encryption.hpp"

using namespace nodcru2;

TEST_CASE("Encrypt / decrypt a message", "[message]" ) {
    std::string key1 = "12345678901234567890123456789012";
    NCEncryption encryption(key1);
    std::string msg1 = "Hello world, this is a test for encrypting a message. Add some more content: test, test, test, test, test, test, test, test.";
    std::vector<uint8_t> msg1v(msg1.begin(), msg1.end());


    auto encrypted_message1 = encryption.nc_encrypt_message(NCDecryptedMessage(msg1v));

    REQUIRE(encrypted_message1.data.size() == msg1.size());

    auto decrypted_message1 = encryption.nc_decrypt_message(encrypted_message1);

    REQUIRE(decrypted_message1.data.size() == msg1.size());

    std::string msg2(decrypted_message1.data.begin(), decrypted_message1.data.end());
    REQUIRE(msg2 == msg1);
}

TEST_CASE("NonEncryption", "[message]" ) {
    std::string key1 = "12345678901234567890123456789012";
    NCNonEncryption non_encryption(key1);
    std::string msg1 = "Hello world, this is a test for encrypting a message. Add some more content: test, test, test, test, test, test, test, test.";
    std::vector<uint8_t> msg1v(msg1.begin(), msg1.end());


    auto encrypted_message1 = non_encryption.nc_encrypt_message(NCDecryptedMessage(msg1v));

    REQUIRE(encrypted_message1.data.size() == msg1.size());

    auto decrypted_message1 = non_encryption.nc_decrypt_message(encrypted_message1);

    REQUIRE(decrypted_message1.data.size() == msg1.size());

    std::string msg2(decrypted_message1.data.begin(), decrypted_message1.data.end());
    REQUIRE(msg2 == msg1);
}
