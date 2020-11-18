#include "WifiComms.h"

int other_socket;

int receive(char buffer[1024]) {
    int status = recv(other_socket, buffer, 1024, 0);

    if (status == -1) {
        int connect_status = connect(8080);
        if (connect_status == -1) {
            std::cerr<<"Error connecting to the socket";
            exit(1);
        }

        recv(other_socket, buffer, 1024, 0);
    }

    return 0;
}

int send(char data[1024]) {
    int status = send(other_socket, data, 1024, 0);

    if (status == -1) {
        int connect_status = connect(8080);
        if (connect_status == -1) {
            std::cerr<<"Error connecting to the socket";
            exit(1);
        }

        send(other_socket, data, 1024, 0);
    }

    return 0;
}

int polling() {

    struct pollfd poll_fd[1];

    poll_fd->fd = 0;
    poll_fd->events = POLLIN;

    int num_events = poll(poll_fd, 1, 10000);

    if (num_events == 0) {
        printf("Poll times out");
    } else {
        char buffer[1024];
        receive(buffer);

        // do something with the data. send it further.
    }

    return 0;
}

int connect(int port) {
    struct addrinfo hints{};
    struct addrinfo *server_info;

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    std::string tmp = std::to_string(port);
    char const *port_char = tmp.c_str();

    if (getaddrinfo(nullptr, port_char, &hints, &server_info) != 0) {
        std::cerr<<"Error retrieving information from server";
        exit(1);
    }

    int socket_fd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
    if (socket_fd == -1) {
        std::cerr<<"Error creating the socket";
        exit(1);
    }

    int bind_status = bind(socket_fd, server_info->ai_addr, server_info->ai_addrlen);
    if (bind_status == -1) {
        std::cerr<<"Error binding the address to the socket";
        exit(1);
    }

//    int yes = 1;
//    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, 1) == -1) {
//        std::cerr<<"Port could not be reused";
//        exit(1);
//    }

    int listen_status = listen(socket_fd, 1);
    if (listen_status == -1) {
        std::cerr<<"Error listening to the port";
        exit(1);
    }

    struct sockaddr_storage second_address{};
    int new_socket_fd = accept(socket_fd, (struct sockaddr *)&second_address, (socklen_t*) sizeof(second_address));

    if (new_socket_fd == -1) {
        std::cerr<<"Error accepting connection";
        exit(1);
    }

    other_socket = new_socket_fd;

    return 0;
}