#include "BluetoothComms.h"
#include <iostream>
#include "bluetooth/bluetooth.h"
#include "bluetooth/rfcomm.h"
#include "sys/socket.h"
#include <unistd.h>
#include <cstring>
#include <openssl/err.h>
#include <pwd.h>
#include "filesystem"

using namespace std;

BluetoothComms::BluetoothComms(bool logging, bool encryption, int channel) : BluetoothComms() {
    this->logging = logging;
    this->channel = channel;
    this->encryption = encryption;
}

BluetoothComms::BluetoothComms(int channel) : BluetoothComms() {
    this->channel = channel;
}

BluetoothComms::BluetoothComms() {
    this->logging = false;
    this->encryption = false;
    server_socket_fd = -1;
    client_socket_fd = -1;
    this->channel = 0;
    ssl = nullptr;
    context = nullptr;
}

int BluetoothComms::send(char *data) {

    int no_of_bytes;

    if (encryption) {
        if (logging) {
            cout << "Sending securely: " << data << endl;
        }
        no_of_bytes = ::SSL_write(ssl, data, strlen(data));

        if (no_of_bytes <= 0) {
            cout << "Error sending the message" << endl;
            return no_of_bytes;
        }

        if (logging) {
            cout << "Successfully sent " << no_of_bytes << " bytes (securely)" << endl;
        }

    } else {
        if (logging) {
            cout << "Sending insecurely: " << data << endl;
        }
        no_of_bytes = ::send(client_socket_fd, data, strlen(data), 0);

        if (no_of_bytes == -1) {
            cout << "Error sending the message" << endl;
            return no_of_bytes;
        }

        if (logging) {
            cout << "Successfully sent " << no_of_bytes << " bytes (insecurely)" << endl;
        }
    }

    return no_of_bytes;
}

int BluetoothComms::receive(char buffer[BUFFER_SIZE]) {

    int no_of_bytes;

    if (encryption) {
        if (logging) {
            cout << "Waiting to receive a secure message" << endl;
        }

        no_of_bytes = SSL_read(ssl, buffer, BUFFER_SIZE);

        if (no_of_bytes > 0) {
            if (logging) {
                cout << "Successfully received " << no_of_bytes << " bytes (securely)" << endl;
            }
        }
        if (no_of_bytes <= 0) {
            if (logging) {
                cout << "An error has occurred or the connection has been closed" << endl;
            }
            disconnect();
        }
    } else {
        if (logging) {
            cout << "Waiting to receive an insecure message" << endl;
        }

        no_of_bytes = ::recv(client_socket_fd, buffer, BUFFER_SIZE, 0);

        if (no_of_bytes == -1) {
            cout << "Error receiving the message" << endl;
            disconnect();
            return no_of_bytes;
        }

        if (no_of_bytes == 0) {
            cout << "Connection was closed" << endl;
            disconnect();
            return no_of_bytes;
        }

        if (logging) {
            cout << "Successfully received " << no_of_bytes << " bytes (insecurely)" << endl;
        }
    }

    return no_of_bytes;
}

int BluetoothComms::disconnect() {

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

        if (encryption) {
            SSL_free(ssl);
            SSL_CTX_free(context);
        }

        return 0;
    }
    catch (exception &exception) {
        cerr << exception.what() << endl;
        return -1;
    }
}

int BluetoothComms::create_socket() {

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
            if (encryption) {
                cout << "Accepting a new secure connection" << endl;
            } else {
                cout<< "Accepting a new insecure connection" << endl;
            }
        }

        struct sockaddr_storage client_address{};
        int address_len = sizeof(client_address);
        int socket_fd = accept(server_socket_fd, (struct sockaddr *) &client_address, (socklen_t *) &address_len);

        if (socket_fd == -1) {
            throw runtime_error("Error accepting incoming connection");
        }

        if (logging) {
            if (encryption) {
                cout << "Successfully accepted " << socket_fd << " (securely)" << endl;
            } else {
                cout << "Successfully accepted " << socket_fd << " (insecurely)" << endl;
            }
        }
        client_socket_fd = socket_fd;

        return 0;
    }
    catch (exception &exception) {
        cerr << exception.what() << endl;
        return -1;
    }
}

int BluetoothComms::load_certificates(SSL_CTX * context, char * certificate_file, char * key_file, char * ca_file) {
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

int BluetoothComms::establish_connection() {

    if (encryption) {
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
        strcat(on_board_cert, "/DFLOW/test_certs/on-board/on-board.crt");

        strcpy(on_board_key, homedir);
        strcat(on_board_key, "/DFLOW/test_certs/on-board/on-board.key");

        strcpy(rootCA, homedir);
        strcat(rootCA, "/DFLOW/test_certs/rootCA/rootCA.crt");

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
    }

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

    if (encryption) {
        ssl = SSL_new(context);
        SSL_set_fd(ssl, client_socket_fd);

        int ssl_status = SSL_accept(ssl);

        if (ssl_status == -1) {
            throw runtime_error("Error in accepting SSL protocol");
        }
    }

    return 0;
}
