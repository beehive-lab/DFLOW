#ifndef DFLOW_BLUETOOTHLOGIC_H
#define DFLOW_BLUETOOTHLOGIC_H

#include <pipes.hpp>
#include "BluetoothComms.h"

class BluetoothLogic {

public:
    explicit BluetoothLogic(std::vector<Pipes> processed_pipes_vector);

    void receive_loop( BluetoothComms, char receive_buffer[BUFFER_SIZE]);

    void read_and_send(BluetoothComms bluetoothComms);

private:

    std::vector<Pipes> processed_pipes_vector;

    bool currently_streaming[18] {false};

    bool starting = false;

    bool stopping = false;

    int type_of_comms = 0;
};


#endif
