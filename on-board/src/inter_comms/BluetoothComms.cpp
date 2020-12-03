#include "BluetoothComms.h"
#include <iostream>
#include "bluetooth/bluetooth.h"
#include "bluetooth/rfcomm.h"
#include "sys/socket.h"
#include <unistd.h>
#include <cstring>

using namespace std;

BluetoothComms::BluetoothComms(bool logging) {
    this->logging = logging;
    server_socket_fd = -1;
    client_socket_fd = -1;
}

int BluetoothComms::send(char *data) {

    if (logging) {
        cout << "Sending: " << data;
    }

    int status = ::send(client_socket_fd, data, strlen(data), 0);

    if (status != 0) {
        if (logging) {
            cerr << "Error sending the message" << endl;
        }
        exit(1);
    }

    return 0;
}

int BluetoothComms::receive(char buffer[BUFFER_SIZE]) {

    int status = recv(client_socket_fd, buffer, 1024, 0);

    if (status != 0) {
        if (logging) {
            cerr << "Error receiving the message" << endl;
        }
        exit(1);
    }

    cout << buffer << endl;

    return 0;
}

int BluetoothComms::disconnect() {

    int status = close(server_socket_fd);

    if (status == -1) {
        if (logging) {
            cerr << "Error closing the socket" << endl;
        }
        exit(1);
    }

    status = close(client_socket_fd);

    if (status == -1) {
        if (logging) {
            cerr << "Error closing the socket" << endl;
        }
        exit(1);
    }

    return 0;
}

int BluetoothComms::listen_to_connection(int channel) {

    struct sockaddr_rc local_address = {0};

    local_address.rc_family = AF_BLUETOOTH;
    local_address.rc_bdaddr = ((bdaddr_t) {{0, 0, 0, 0, 0, 0}});
    local_address.rc_channel = (uint8_t) channel;

    int socket_fd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    if (socket_fd == -1) {
        if (logging) {
            cerr << "Error creating the socket" << endl;
        }
        exit(1);
    }

    server_socket_fd = socket_fd;

    int bind_status = bind(socket_fd, (struct sockaddr *) &local_address, sizeof(local_address));
    if (bind_status == -1) {
        if (logging) {
            cerr << "Error binding the address to the socket" << endl;
        }
        exit(1);
    }

    int listen_status = listen(socket_fd, 1);
    if (listen_status == -1) {
        if (logging) {
            cerr << "Error listening to the port" << endl;
        }
        exit(1);
    }

    struct sockaddr_rc comm_address = {0};
    int address_len = sizeof(comm_address);
    int new_socket_fd = accept(socket_fd, (struct sockaddr *) &comm_address, (socklen_t *) &address_len);

    if (new_socket_fd == -1) {
        if (logging) {
            cerr << "Error accepting connection" << endl;
        }
        exit(1);
    }

    client_socket_fd = new_socket_fd;

    return 0;
}
