#ifndef DFLOW_SYMMETRICENCRYPTION_H
#define DFLOW_SYMMETRICENCRYPTION_H


#include <stdint-gcc.h>
#include <openssl/aes.h>

/**
 AES encryption (CBC mode) to be used internally. The on-board data can be
 encrypted for an extra layer of security. This functionality can be turned
 on or off by setting the flag in Logic or BluetoothLogic.
 */
class SymmetricEncryption {
public:

    /**
     * Encrypts the given string using the key provided.
     * @param key The key to be used when encrypting the source string.
     * @param size The size of the source string.
     * @param source The string to be encrypted using AES.
     * @param target The result of the encryption operation will be stored here.
     */
    static void encrypt(uint8_t key[], int size, const uint8_t source[], uint8_t target[]);

    /**
     * Decrypts the ciphertext using the key provided.
     * @param key The key to used then decrypting.
     * @param size The size of the cipher text.
     * @param source The ciphertext to be decrypted.
     * @param target The original string, obtained after a decryption operation.
     */
    static void decrypt(uint8_t key[], int size, uint8_t source[], uint8_t target[]);

private:

    /**
     * The randomly generated initialization vector is used in the AES algorithm
     * to provide an initial state, required when performing the encryption.
     */
    static uint8_t initialization_vector[AES_BLOCK_SIZE];

    /**
     * Helper variable to store the status of the initialization vector.
     */
    static bool initialized_vector;

    /**
     * Only called once. Initializes the vector with random values for a better
     * level of encryption.
     */
    static void initialize_IV();
};

#endif
