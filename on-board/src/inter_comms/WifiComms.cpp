#include "WifiComms.h"
#include <cstring>

using namespace std;

int other_socket;

int this_socket;

int send(char *data) {
    int status = send(other_socket, data, strlen(data), 0);

    if (status == -1) {
        int connect_status = listen_to_connection(8080);
        if (connect_status == -1) {
            cerr << "Error connecting to the socket" << endl;
            exit(1);
        }

        send(other_socket, data, strlen(data), 0);
    }

    return 0;
}

int polling() {

    struct pollfd poll_fd[1];

    poll_fd[0].fd = other_socket;
    poll_fd[0].events = POLLIN;

//    poll(poll_fd, 1, -1);

    char buffer[1024];
//    if (poll_fd[0].revents & POLLIN) {
    recv(other_socket, buffer, 1024, 0);
//    }

    cout << buffer << endl;

    send(buffer);

    // do something with the data. send it further.

    return 0;
}

int listen_to_connection(int port) {
    struct addrinfo hints{};
    struct addrinfo *server_info;

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    string tmp = to_string(port);
    char const *port_char = tmp.c_str();

    if (getaddrinfo(nullptr, port_char, &hints, &server_info) != 0) {
        cerr << "Error retrieving information from server" << endl;
        exit(1);
    }

    int socket_fd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
    if (socket_fd == -1) {
        cerr << "Error creating the socket" << endl;
        exit(1);
    }

    this_socket = socket_fd;

    int bind_status = bind(socket_fd, server_info->ai_addr, server_info->ai_addrlen);
    if (bind_status == -1) {
        cerr << "Error binding the address to the socket" << endl;
        exit(1);
    }

//    int yes = 1;
//    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, 1) == -1) {
//        cerr<<"Port could not be reused";
//        exit(1);
//    }

    int listen_status = listen(socket_fd, 1);
    if (listen_status == -1) {
        cerr << "Error listening to the port" << endl;
        exit(1);
    }

    struct sockaddr_storage second_address{};
    int address_len = sizeof(second_address);
    int new_socket_fd = accept(socket_fd, (struct sockaddr *) &second_address, (socklen_t *) &address_len);

    if (new_socket_fd == -1) {
        cerr << "Error accepting connection" << endl;
        exit(1);
    }

    other_socket = new_socket_fd;

    return 0;
}