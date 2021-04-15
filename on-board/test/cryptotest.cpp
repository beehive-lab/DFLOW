#include <crypto/SymmetricEncryption.h>
#include <string>
#include "gtest/gtest.h"

namespace {

    class CryptoTest : public ::testing::Test {
    };

    TEST_F(CryptoTest, EncryptDecrypt){
        uint8_t source[6] = "hello";
        auto *target = new uint8_t[256];
        uint8_t result[6];

        uint8_t key[7] = "my_key";

        SymmetricEncryption::encrypt(key, sizeof source, source, target);

        SymmetricEncryption::decrypt(key, sizeof target, target, result);

        EXPECT_TRUE(
                strcmp(reinterpret_cast<const char *>(source), reinterpret_cast<const char *>(result)) == 0);

        uint8_t wrong_key[8] = "my_key2";

        SymmetricEncryption::decrypt(wrong_key, sizeof target, target, result);

        EXPECT_FALSE(
                strcmp(reinterpret_cast<const char *>(source), reinterpret_cast<const char *>(result)) == 0);
    }

}
