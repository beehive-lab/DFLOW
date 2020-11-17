#ifndef DFLOW_WIFICOMMS_H
#define DFLOW_WIFICOMMS_H

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

int receive(int port);

int send(int port, char data[1024]);

#endif
