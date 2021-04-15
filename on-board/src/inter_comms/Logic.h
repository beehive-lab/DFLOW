#ifndef DFLOW_LOGIC_H
#define DFLOW_LOGIC_H

#include <pipes.hpp>
#include "WifiComms.h"
#include "BluetoothComms.h"
#include <on_board_data_interface.hpp>
#include <fstream>

class Logic {

public:

    explicit Logic(OnBoardDataInterface* data_interface, bool store_locally, bool encrypt_locally, uint8_t key[10]);

    void Wifi_logic(bool logging, bool encryption, int port);

    void Bluetooth_logic(bool logging, bool encryption, int channel);

private:

    void receive_loop(WifiComms *wifiComms, char receive_buffer[BUFFER_SIZE]);

    void send_bike_metrics(WifiComms wifiComms);

    void send_profiling_data(WifiComms wifiComms);

    OnBoardDataInterface* data_interface;

    bool currently_streaming[26] {false};

    bool starting = false;

    bool stopping = false;

    bool exit_application = false;

    int type_of_comms = 0;

    bool store_locally = false;

    bool encrypt_locally;

    uint8_t *key;

    std::ofstream file;

    static void start_bandwidth_test(WifiComms *wifi_comms);
};


#endif
