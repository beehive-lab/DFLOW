#ifndef DFLOW_BLUETOOTHCOMMS_H
#define DFLOW_BLUETOOTHCOMMS_H

#include "Communications.h"
#include "openssl/ssl.h"

class BluetoothComms: public Communications {
public:
    explicit BluetoothComms(bool logging, int channel);

    explicit BluetoothComms();

    int send(char *data) override;

    int receive(char buffer[BUFFER_SIZE]) override;

    int disconnect() override;

    int establish_connection(int channel) override;

    int create_socket(int port) override;

    int bind_socket() override;

    int accept_connection() override;

    int listen_socket() override;

private:
    int server_socket_fd;

    int client_socket_fd;

    bool logging;

    int channel;

    SSL *ssl;

    SSL_CTX *context;

    int load_certificates(SSL_CTX * context, char * certificate_file, char * key_file, char * ca_file);
};


#endif
