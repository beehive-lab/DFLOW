#ifndef DFLOW_SYMMETRICENCRYPTION_H
#define DFLOW_SYMMETRICENCRYPTION_H


#include <stdint-gcc.h>
#include <openssl/aes.h>

class SymmetricEncryption {
public:

    static void encrypt(uint8_t key[], int size, const uint8_t source[], uint8_t target[]);

    static void decrypt(uint8_t key[], int size, uint8_t source[], uint8_t target[]);

private:
    static uint8_t initialization_vector[AES_BLOCK_SIZE];

    static bool initialized_vector;

    static void initialize_IV();
};

#endif
