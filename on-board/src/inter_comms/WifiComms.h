#ifndef DFLOW_WIFICOMMS_H
#define DFLOW_WIFICOMMS_H

#include "Communications.h"

class WifiComms : public Communications {
public:
    explicit WifiComms(bool logging);

    int send(char *data) override;

    int receive(char buffer[BUFFER_SIZE]) override;

    int disconnect() override;

    int listen_to_connection(int port) override;

private:
    int other_socket;

    int this_socket;

    bool logging;
};

#endif
