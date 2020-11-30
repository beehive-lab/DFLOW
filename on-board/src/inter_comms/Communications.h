#ifndef DFLOW_COMMUNICATIONS_H
#define DFLOW_COMMUNICATIONS_H

#define BUFFER_SIZE 1024

class Communications {
public:
    virtual int send(char *data) = 0;

    virtual int listen_to_connection(int port) = 0;

    virtual int receive(char buffer[1024]) = 0;

    virtual int disconnect() = 0;
};

#endif
