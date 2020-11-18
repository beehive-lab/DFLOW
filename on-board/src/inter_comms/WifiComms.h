#ifndef DFLOW_WIFICOMMS_H
#define DFLOW_WIFICOMMS_H

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <poll.h>

extern int other_socket;

int receive();

int send(char data[1024]);

int polling();

int connect(int port);

#endif
