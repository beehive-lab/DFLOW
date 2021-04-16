#ifndef DFLOW_COMMUNICATIONS_H
#define DFLOW_COMMUNICATIONS_H

#define BUFFER_SIZE 1024

#include "config.hpp"

/**
 * The communications superclass. This project offers WiFi and Bluetooth
 * communications, with similar functionality on both sides. If any other
 * means of communications is added in the future, it should extend this class.
 */
class Communications {
public:
    /**
     * Send the argument given to the client socket.
     * @param data char pointer to send.
     * @return number of bytes sent. If unsuccessful, returns -1.
     */
    virtual int send(char *data) = 0;

    /**
     * Establishes the WiFi/BT connection. Should be called when
     * connecting or reconnecting.
     * @return 0 if the connection was established, -1 otherwise.
     */
    virtual int establish_connection() = 0;

    /**
     * Blocks the current thread and waits to receive a message
     * from the client through the socket. The received data
     * is stored in the buffer.
     * @param buffer the buffer to store received data.
     * @return the number of bytes received. If unsuccessful, a
     * negative value.
     */
    virtual int receive(char buffer[BUFFER_SIZE]) = 0;

    /**
     * Disconnects from the client. It closes the client communication
     * and removes SSL data if encryption was turned on.
     * @return 0 if successful, -1 otherwise.
     */
    virtual int disconnect() = 0;

    /**
     * Creates a new reused socket, with no timeout.
     * @return 0 if successful, -1 otherwise.
     */
    virtual int create_socket() = 0;

    /**
     * Binds the socket to the address.
     * @return 0 if successful, -1 otherwise.
     */
    virtual int bind_socket() = 0;

    /**
     * Accepts a new connection.
     * @return 0 if successful, -1 otherwise.
     */
    virtual int accept_connection() = 0;

    /**
     * Listens to the socket. Required in establishing the connection.
     * @return 0 if successful, -1 otherwise.
     */
    virtual int listen_socket() = 0;
};

#endif
