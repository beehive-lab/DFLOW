#ifndef DFLOW_WIFICOMMS_H
#define DFLOW_WIFICOMMS_H

#include "Communications.h"

class WifiComms : public Communications {
public:
    explicit WifiComms(bool logging);

    int send(char *data) override;

    int receive(char buffer[BUFFER_SIZE]) override;

    int disconnect() override;

    int establish_connection(int port) override;

    int create_socket(int port) override;

    int bind_socket() override;

    int accept_connection() override;

    int listen_socket() override;

private:
    struct addrinfo *server_info;

    int client_socket_fd;

    int server_socket_fd;

    bool logging;
};

#endif
