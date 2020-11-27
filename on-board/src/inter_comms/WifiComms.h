#ifndef DFLOW_WIFICOMMS_H
#define DFLOW_WIFICOMMS_H

#include "Communications.h"

class WifiComms: public Communications {
public:
    WifiComms();

    int send(char *data) override;

    int receive(char buffer[1024]) override;

    int disconnect() override;

    int listen_to_connection(int port) override;

private:
    int other_socket;

    int this_socket;
};

#endif
