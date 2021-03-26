#include <crypto/SymmetricEncryption.h>
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

        EXPECT_EQ(source, result);

        uint8_t wrong_key[8] = "my_key2";

        SymmetricEncryption::decrypt(wrong_key, sizeof target, target, result);

        EXPECT_NE(source, result);
    }

}
