#ifndef DFLOW_COMMUNICATIONS_H
#define DFLOW_COMMUNICATIONS_H

#define BUFFER_SIZE 1024

class Communications {
public:
    virtual int send(char *data) = 0;

    virtual int establish_connection() = 0;

    virtual int receive(char buffer[BUFFER_SIZE]) = 0;

    virtual int disconnect() = 0;

    virtual int create_socket() = 0;

    virtual int bind_socket() = 0;

    virtual int accept_connection() = 0;

    virtual int listen_socket() = 0;
};

#endif
