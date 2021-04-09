#include "WifiComms.h"
#include "config.hpp"
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <unistd.h>
#include <openssl/err.h>
#include <pwd.h>
#include "filesystem"

using namespace std;

WifiComms::WifiComms(bool logging, int port) : WifiComms() {
    this->logging = logging;
    this->port = port;
}

WifiComms::WifiComms(int port) : WifiComms() {
    this->port = port;
}

WifiComms::WifiComms() {
    this->port = 8080;
    this->logging = false;
    server_socket_fd = -1;
    client_socket_fd = -1;
    server_info = nullptr;
    ssl = nullptr;
    context = nullptr;
};

int WifiComms::send(char *data) {
    if (logging) {
        cout << "Sending: " << data << endl;
    }

    int no_of_bytes = ::SSL_write(ssl, data, strlen(data));


    if (no_of_bytes <= 0) {
        cout<<"Error sending the message. Trying to send again."<<endl;
        return no_of_bytes;
    }

    if (logging) {
        cout << "Successfully sent " << no_of_bytes << " bytes" << endl;
    }

    return no_of_bytes;
}

int WifiComms::receive(char buffer[BUFFER_SIZE]) {
    if (logging) {
        cout << "Waiting to receive a message" << endl;
    }

    int no_of_bytes = SSL_read(ssl, buffer, BUFFER_SIZE);

    if (no_of_bytes > 0) {
        if (logging) {
            cout << "Successfully received " << no_of_bytes << " bytes" << endl;
        }
    }
    if (no_of_bytes <= 0) {
        if (logging) {
            cout<< "An error has occurred or the connection has been closed" << endl;
        }
        disconnect();
    }

    return no_of_bytes;
}

int WifiComms::disconnect() {

    try {

        if (logging) {
            cout<<"Disconnecting..."<<endl;
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

        SSL_free(ssl);
        SSL_CTX_free(context);

        return 0;
    }
    catch (exception &exception) {
        cerr << exception.what() << endl;
        return -1;
    }
}

int WifiComms::create_socket() {

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

int WifiComms::load_certificates(SSL_CTX * context, char * certificate_file, char * key_file, char * ca_file) {
    try {
        if (SSL_CTX_use_certificate_file(context, certificate_file, SSL_FILETYPE_PEM) <= 0) {
            ERR_print_errors_fp(stderr);
            throw runtime_error("Error loading certificate file");
        }

        if (SSL_CTX_use_PrivateKey_file(context, key_file, SSL_FILETYPE_PEM) <= 0) {
            throw runtime_error("Error loading private key file");
        }

        if (!SSL_CTX_check_private_key(context)) {
            throw runtime_error("Private key doesn't match public key certificate");
        }

        if (SSL_CTX_load_verify_locations(context, ca_file, nullptr) <= 0) {
            throw runtime_error("Error loading CA certificate");
        }

        return 0;
    }
    catch (exception &exception) {
        cerr << exception.what() << endl;
        return -1;
    }
}

int WifiComms::establish_connection() {

    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    const SSL_METHOD *method = SSLv23_server_method();
    context = SSL_CTX_new(method);

    if (!context) {
        return -1;
    }

    const char *homedir = DFLOW_HOME;

    char on_board_cert[256], on_board_key[256], rootCA[256];

    strcpy(on_board_cert, homedir);
    strcat(on_board_cert, "/test_certs/on-board/on-board.crt");

    strcpy(on_board_key, homedir);
    strcat(on_board_key, "/test_certs/on-board/on-board.key");

    strcpy(rootCA, homedir);
    strcat(rootCA, "/test_certs/rootCA/rootCA.crt");


    if (!filesystem::exists(on_board_cert)) {
        throw runtime_error("Certificate file doesn't exist");
    }

    if (!filesystem::exists(on_board_key)) {
        throw runtime_error("Private key file doesn't exist");
    }

    if (!filesystem::exists(rootCA)) {
        throw runtime_error("CA certificate doesn't exist");
    }

    load_certificates(context, on_board_cert, on_board_key, rootCA);

    int socket_creation_status = create_socket();
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

    ssl = SSL_new(context);
    SSL_set_fd(ssl, client_socket_fd);

    int ssl_status = SSL_accept(ssl);

    if (ssl_status == -1) {
        throw runtime_error("Error in accepting SSL protocol");
    }

    return 0;
}