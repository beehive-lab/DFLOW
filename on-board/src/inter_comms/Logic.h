#ifndef DFLOW_LOGIC_H
#define DFLOW_LOGIC_H

#include <pipes.hpp>
#include "WifiComms.h"
#include "BluetoothComms.h"


class Logic {

public:

    explicit Logic(std::vector<Pipes> processed_pipes_vector);

    void Wifi_logic(bool logging, bool encryption, int port);

    void Bluetooth_logic(bool logging, bool encryption, int channel);

private:

    void receive_loop(WifiComms *wifiComms, char receive_buffer[BUFFER_SIZE]);

    void read_and_send(WifiComms wifiComms);

    std::vector<Pipes> processed_pipes_vector;

    bool currently_streaming[18] {false};

    bool starting = false;

    bool stopping = false;

    bool exit_application = false;

    int type_of_comms = 0;
};


#endif
