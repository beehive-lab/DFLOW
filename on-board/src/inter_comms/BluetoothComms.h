#ifndef DFLOW_BLUETOOTHCOMMS_H
#define DFLOW_BLUETOOTHCOMMS_H

#include "Communications.h"

class BluetoothComms: public Communications {
public:
    explicit BluetoothComms(bool logging);

    int send(char *data) override;

    int receive(char buffer[BUFFER_SIZE]) override;

    int disconnect() override;

    int listen_to_connection(int port) override;

private:
    int server_socket_fd;

    int client_socket_fd;

    bool logging;
};


#endif
