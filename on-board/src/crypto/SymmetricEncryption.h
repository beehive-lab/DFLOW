#ifndef DFLOW_SYMMETRICENCRYPTION_H
#define DFLOW_SYMMETRICENCRYPTION_H


#include <stdint-gcc.h>
#include <openssl/aes.h>

/**
 AES encryption (CBC mode) to be used internally. The on-board data can be
 encrypted for an extra layer of security.

 The randomly generated initialization vector is used in the AES algorithm
 to provide an initial state, required when performing the encryption.
 */
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
