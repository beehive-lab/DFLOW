#ifndef DFLOW_BLUETOOTHCOMMS_H
#define DFLOW_BLUETOOTHCOMMS_H

#include "Communications.h"
#include "openssl/ssl.h"

class BluetoothComms: public Communications {
public:
    explicit BluetoothComms(bool logging, bool encryption, int channel);

    explicit BluetoothComms(int channel);

    explicit BluetoothComms();

    void set_encryption(bool encryption_set);

    int send(char *data) override;

    int receive(char buffer[BUFFER_SIZE]) override;

    int disconnect() override;

    int establish_connection() override;

    int create_socket() override;

    int bind_socket() override;

    int accept_connection() override;

    int listen_socket() override;

    /**
     * Logging can be turned on or off.
     */
    bool logging;

private:
    /**
     * The on-board socket file descriptor. It always remains open.
     */
    int server_socket_fd;

    /**
     * The client socket file descriptor. Used when communicating with it.
     */
    int client_socket_fd;

    /**
     * The encryption can be turned on or off.
     */
    bool encryption;

    /**
     * The channel that this connection is communicating through.
     */
    int channel;

    SSL *ssl;

    SSL_CTX *context;

    /**
     * Loads the certificate files if the encryption is turned on.
     * @param context the current ssl communication
     * @param certificate_file the path to the certificate file
     * @param key_file the path to the file that holds the private key
     * @param ca_file the path to the certificate file of the CA authority.
     * @return 0 if successful, -1 otherwise.
     */
    static int load_certificates(SSL_CTX * context, char * certificate_file, char * key_file, char * ca_file);
};


#endif
