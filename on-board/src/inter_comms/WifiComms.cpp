#include "WifiComms.h"
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <unistd.h>

using namespace std;


int WifiComms::send(char *data) {

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

int WifiComms::receive(char buffer[BUFFER_SIZE]) {

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

int WifiComms::disconnect() {

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

int WifiComms::create_socket(int port) {

    try {
        struct addrinfo hints{};

        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;

        string tmp = to_string(port);
        char const *port_char = tmp.c_str();

        if (logging) {
            cout << "Retrieving server information" << endl;
        }

        if (getaddrinfo(nullptr, port_char, &hints, &server_info) != 0) {
            throw runtime_error("Error retrieving server information");
        }

        if (logging) {
            cout << "Creating the server socket" << endl;
        }

        int socket_fd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
        if (socket_fd == -1) {
            throw runtime_error("Error creating the server socket");
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

int WifiComms::bind_socket() {
    try {
        if (logging) {
            cout << "Binding the address to the socket" << endl;
        }

        int bind_status = bind(server_socket_fd, server_info->ai_addr, server_info->ai_addrlen);
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

int WifiComms::listen_socket() {
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

int WifiComms::accept_connection() {
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

int WifiComms::establish_connection(int port) {

    int socket_creation_status = create_socket(port);
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

WifiComms::WifiComms(bool logging) : WifiComms() {
    this->logging = logging;
}

WifiComms::WifiComms() {
    this->logging = false;
    server_socket_fd = -1;
    client_socket_fd = -1;
    server_info = nullptr;
};
