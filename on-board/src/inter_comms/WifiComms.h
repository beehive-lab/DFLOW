#ifndef DFLOW_WIFICOMMS_H
#define DFLOW_WIFICOMMS_H

#include "Communications.h"
#include "openssl/ssl.h"

class WifiComms : public Communications {
public:
    explicit WifiComms(bool logging, int port);

    explicit WifiComms(int port);

    explicit WifiComms();

    int send(char *data) override;

    int receive(char buffer[BUFFER_SIZE]) override;

    int disconnect() override;

    int establish_connection() override;

    int create_socket() override;

    int bind_socket() override;

    int accept_connection() override;

    int listen_socket() override;

private:
    int port;

    struct addrinfo *server_info;

    int client_socket_fd;

    int server_socket_fd;

    bool logging;

    SSL *ssl;

    SSL_CTX *context;

    static int load_certificates(SSL_CTX * context, char * certificate_file, char * key_file, char * ca_file);
};

#endif
