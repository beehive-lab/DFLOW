#ifndef DFLOW_LOGIC_H
#define DFLOW_LOGIC_H

#include <pipes.hpp>
#include "WifiComms.h"


class Logic {

public:

    explicit Logic(std::vector<Pipes> processed_pipes_vector);

    void Wifi_logic(bool logging, int port);

    void Bluetooth_logic(bool logging, int channel);

private:

    static void receive_loop(WifiComms wifiComms, char receive_buffer[BUFFER_SIZE]);

    static std::vector<Pipes> processed_pipes_vector;

};


#endif
