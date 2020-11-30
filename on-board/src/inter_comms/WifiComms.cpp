#include "WifiComms.h"
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <cstdlib>
#include <iostream>
#include <unistd.h>

using namespace std;


int WifiComms::send(char *data) {

    cout << "Sending: " << data;

    ::send(other_socket, data, strlen(data), 0);

    return 0;
}

int WifiComms::receive(char buffer[BUFFER_SIZE]) {

    recv(other_socket, buffer, 1024, 0);

    cout << buffer << endl;

    return 0;
}

int WifiComms::disconnect() {

    int status = close(this_socket);

    if (status == -1) {
        if (logging) {
            cerr << "Error closing the socket" << endl;
        }
        exit(1);
    }

    status = close(other_socket);

    if (status == -1) {
        if (logging) {
            cerr << "Error closing the socket" << endl;
        }
        exit(1);
    }

    return 0;
}

int WifiComms::listen_to_connection(int port) {
    struct addrinfo hints{};
    struct addrinfo *server_info;

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    string tmp = to_string(port);
    char const *port_char = tmp.c_str();

    if (getaddrinfo(nullptr, port_char, &hints, &server_info) != 0) {
        if (logging) {
            cerr << "Error retrieving information from server" << endl;
        }
        exit(1);
    }

    int socket_fd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
    if (socket_fd == -1) {
        if (logging) {
            cerr << "Error creating the socket" << endl;
        }
        exit(1);
    }

    this_socket = socket_fd;

    int bind_status = bind(socket_fd, server_info->ai_addr, server_info->ai_addrlen);
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

    struct sockaddr_storage second_address{};
    int address_len = sizeof(second_address);
    int new_socket_fd = accept(socket_fd, (struct sockaddr *) &second_address, (socklen_t *) &address_len);

    if (new_socket_fd == -1) {
        if (logging) {
            cerr << "Error accepting connection" << endl;
        }
        exit(1);
    }

    other_socket = new_socket_fd;

    return 0;
}

WifiComms::WifiComms(bool logging) {
    this->logging = logging;
    this_socket = -1;
    other_socket = -1;
}
