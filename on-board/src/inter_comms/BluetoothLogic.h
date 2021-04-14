#ifndef DFLOW_BLUETOOTHLOGIC_H
#define DFLOW_BLUETOOTHLOGIC_H

#include <pipes.hpp>
#include "BluetoothComms.h"
#include <on_board_data_interface.hpp>
#include <fstream>

class BluetoothLogic {

public:
    explicit BluetoothLogic(OnBoardDataInterface* data_interface, bool store_locally, bool encrypt_locally, uint8_t *key);

    void send_profiling_data(BluetoothComms bluetoothComms);

    void receive_loop(BluetoothComms *bluetooth_comms, char receive_buffer[BUFFER_SIZE]);

    void send_bike_metrics(BluetoothComms bluetoothComms);

    bool stopping = false;

    bool exit_application = false;

    std::ofstream file;

private:

    OnBoardDataInterface* data_interface;

    bool currently_streaming[26] {false};

    bool starting = false;

    int type_of_comms = 0;

    bool store_locally = false;

    bool encrypt_locally{};

    uint8_t *key{};
};


#endif
