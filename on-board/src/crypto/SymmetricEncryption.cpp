#include "SymmetricEncryption.h"

#include <openssl/aes.h>
#include <openssl/rand.h>
#include <string>
#include <vector>

uint8_t SymmetricEncryption::initialization_vector[] = {};
bool SymmetricEncryption::initialized_vector = false;

void SymmetricEncryption::initialize_IV() {
    if (!initialized_vector) {
        RAND_bytes(initialization_vector, sizeof(initialization_vector));
        initialized_vector = true;
    }
}

void SymmetricEncryption::encrypt(uint8_t key[], int size, const uint8_t source[], uint8_t target[]) {

    initialize_IV();

    uint8_t IV_copy[AES_BLOCK_SIZE];
    for (int i = 0; i < AES_BLOCK_SIZE; i++) {
        IV_copy[i] = initialization_vector[i];
    }

    auto* aes_key = new AES_KEY();
    AES_set_encrypt_key(key, 256, aes_key);

    std::string to_encrypt(reinterpret_cast<const char *>(source));

    uint8_t required_padding = (AES_BLOCK_SIZE - (to_encrypt.length() % AES_BLOCK_SIZE));

    std::vector<uint8_t> padded_text(to_encrypt.begin(), to_encrypt.end());

    for (int i = 0; i < required_padding; i++) {
        padded_text.push_back(0);
    }

    uint8_t *data = &padded_text[0];
    size = padded_text.size();

    AES_cbc_encrypt(data, target, size, (const AES_KEY*) aes_key, IV_copy, AES_ENCRYPT);
}

void SymmetricEncryption::decrypt(uint8_t key[], int size, uint8_t source[], uint8_t target[]) {

    initialize_IV();

    auto* aes_decrypt_key= new AES_KEY();
    AES_set_decrypt_key(key, 256, aes_decrypt_key);

    uint8_t IV_copy[AES_BLOCK_SIZE];
    for (int i = 0; i < AES_BLOCK_SIZE; i++) {
        IV_copy[i] = initialization_vector[i];
    }

    AES_cbc_encrypt(source, target, size, (const AES_KEY*) aes_decrypt_key, IV_copy, AES_DECRYPT);
}