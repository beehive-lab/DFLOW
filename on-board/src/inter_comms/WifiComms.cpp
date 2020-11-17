#include "WifiComms.h"

int receive(int port) {
    int sockfd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int opt = 1;
    char buffer[1024] = {0};
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == 0) {
        perror("Socket Connection Failed");
        exit(EXIT_FAILURE);
    }

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    bind(sockfd, (struct sockaddr *) &address, sizeof(address));

    listen(sockfd, 3);

    new_socket = accept(sockfd, ((struct sockaddr *) &address), (socklen_t* )&addrlen);

    int values = read(new_socket, buffer, 1024);
    printf("%s", buffer);

    return 0;
}

int send(int port, char data[1024]) {
    int sockfd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int opt = 1;
    char buffer[1024] = {0};
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == 0) {
        perror("Socket Connection Failed");
        exit(EXIT_FAILURE);
    }

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    bind(sockfd, (struct sockaddr *) &address, sizeof(address));

    listen(sockfd, 3);

    new_socket = accept(sockfd, ((struct sockaddr *) &address), (socklen_t* )&addrlen);

    send(new_socket, data, strlen(data), 0);

    return 0;
}