#include "BluetoothComms.h"
#include <iostream>
#include "bluetooth/bluetooth.h"
#include "bluetooth/rfcomm.h"
#include "sys/socket.h"
#include <unistd.h>
#include <cstring>

using namespace std;

BluetoothComms::BluetoothComms(bool logging, int channel) : BluetoothComms() {
    this->logging = logging;
    this->channel = channel;
}

BluetoothComms::BluetoothComms() {
    this->logging = false;
    server_socket_fd = -1;
    client_socket_fd = -1;
    this->channel = 0;
}

int BluetoothComms::send(char *data) {

    try {
        if (logging) {
            cout << "Sending: " << data;
        }

        int no_of_bytes = ::send(client_socket_fd, data, strlen(data), 0);

        if (no_of_bytes == -1) {
            throw runtime_error("Error sending the message");
        }

        if (logging) {
            cout << "Successfully sent " << no_of_bytes << " bytes" << endl;
        }

        return no_of_bytes;
    }
    catch (exception &exception) {
        cerr << exception.what() << endl;
        return -1;
    }
}

int BluetoothComms::receive(char buffer[BUFFER_SIZE]) {

    try {
        if (logging) {
            cout << "Waiting to receive a message" << endl;
        }

        int no_of_bytes = recv(client_socket_fd, buffer, 1024, 0);

        if (no_of_bytes == -1) {
            throw runtime_error("Error receiving the message");
        }

        if (no_of_bytes == 0) {
            throw runtime_error("Connection was closed");
        }

        if (logging) {
            cout << "Successfully received " << no_of_bytes << " bytes" << endl;
        }

        return no_of_bytes;
    }
    catch (exception &exception) {
        cerr << exception.what() << endl;
        return -1;
    }
}

int BluetoothComms::disconnect() {

    try {

        if (logging) {
            cout << "Closing the server socket" << endl;
        }

        int status = close(server_socket_fd);

        if (status == -1) {
            throw runtime_error("Error closing the server socket");
        }

        if (logging) {
            cout << "Successfully closed the server socket" << endl;
        }

        if (logging) {
            cout << "Closing the client socket" << endl;
        }

        status = close(client_socket_fd);

        if (status == -1) {
            throw runtime_error("Error closing the client socket");
        }

        if (logging) {
            cout << "Successfully closed the client socket" << endl;
        }

        return 0;
    }
    catch (exception &exception) {
        cerr << exception.what() << endl;
        return -1;
    }
}

int BluetoothComms::create_socket(int port) {

    try {
        if (logging) {
            cout << "Creating the server socket" << endl;
        }

        struct sockaddr_rc local_address = {0};

        local_address.rc_family = AF_BLUETOOTH;
        local_address.rc_bdaddr = ((bdaddr_t) {{0, 0, 0, 0, 0, 0}});
        local_address.rc_channel = (uint8_t) this->channel;

        int socket_fd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
        if (socket_fd == -1) {
            if (logging) {
                cerr << "Error creating the socket" << endl;
            }
            exit(1);
        }

        if (logging) {
            cout << "Successfully created a new socket" << endl;
        }

        server_socket_fd = socket_fd;

        return 0;
    }
    catch (exception &exception) {
        cerr << exception.what() << endl;
        return -1;
    }
}

int BluetoothComms::bind_socket() {
    try {
        if (logging) {
            cout << "Binding the address to the socket" << endl;
        }

        struct sockaddr_rc local_address = {0};

        local_address.rc_family = AF_BLUETOOTH;
        local_address.rc_bdaddr = ((bdaddr_t) {{0, 0, 0, 0, 0, 0}});
        local_address.rc_channel = (uint8_t) channel;

        int bind_status = bind(server_socket_fd, (struct sockaddr *) &local_address, sizeof(local_address));
        if (bind_status == -1) {
            throw runtime_error("Error binding the address to the socket");
        }

        if (logging) {
            cout << "Successfully bound address to the socket" << endl;
        }

        return 0;
    }
    catch (exception &exception) {
        cerr << exception.what() << endl;
        return -1;
    }
}

int BluetoothComms::listen_socket() {
    try {
        if (logging) {
            cout << "Listening to the socket" << endl;
        }

        int listen_status = listen(server_socket_fd, 1);
        if (listen_status == -1) {
            throw runtime_error("Error listening to the socket");
        }

        return 0;
    }
    catch (exception &exception) {
        cerr << exception.what() << endl;
        return -1;
    }
}

int BluetoothComms::accept_connection() {
    try {
        if (logging) {
            cout << "Accepting a new connection" << endl;
        }

        struct sockaddr_storage client_address{};
        int address_len = sizeof(client_address);
        int socket_fd = accept(server_socket_fd, (struct sockaddr *) &client_address, (socklen_t *) &address_len);

        if (socket_fd == -1) {
            throw runtime_error("Error accepting incoming connection");
        }

        if (logging) {
            cout << "Successfully accepted " << socket_fd << endl;
        }
        client_socket_fd = socket_fd;

        return 0;
    }
    catch (exception &exception) {
        cerr << exception.what() << endl;
        return -1;
    }
}

int BluetoothComms::establish_connection(int port) {

    int socket_creation_status = create_socket(channel);
    if (socket_creation_status == -1) {
        return -1;
    }

    int bind_status = bind_socket();
    if (bind_status == -1) {
        return -1;
    }

    int listen_status = listen_socket();
    if (listen_status == -1) {
        return -1;
    }

    int accept_status = accept_connection();
    if (accept_status == -1) {
        return -1;
    }

    return 0;
}
