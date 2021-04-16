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

    /**
     * The thread that runs this loop received messages from the client,
     * parsing and processing them.
     * @param wifiComms The current WiFi communications. Needs to
     * be established beforehand.
     * @param receive_buffer The buffer that received is stored in.
     */
    void receive_loop(WifiComms *wifiComms, char receive_buffer[BUFFER_SIZE]);

    /**
     * Sends the sensor data read from the pipes to the client, if requested.
     * Also, the sent data is encrypted and stored locally if the flags are set.
     * @param wifiComms The current WiFi communications required to send.
     */
    void send_bike_metrics(WifiComms wifiComms);


    /**
     * Sends profiling data from the profiling module to the client. It
     * includes metrics such as CPU usage or CPU temperature.
     * @param wifiComms
     */
    void send_profiling_data(WifiComms wifiComms);

    /**
     * Used to provide this class with data from the pipes.
     */
    OnBoardDataInterface* data_interface;

    /**
     * Holds the streaming status of each pipe.
     */
    bool currently_streaming[26] {false};

    bool starting = false;

    /**
     * Used to stop the execution of the threads when the command is sent.
     */
    bool stopping = false;

    bool exit_application = false;

    /**
     * Can be:
     * 0 if streaming sensor data.
     * 1 if configuring the pipes.
     * 2 if encryption level is updated remotely.
     * 3 if currently performing a throughput test.
     * 4 if currently streaming profiling data.
     * 5 if currently performing sorting (NlogN) benchmark.
     * If streaming both profiling and sensor data, it can be either of 0 or 4.
     */
    int type_of_comms = 0;

    /**
     * Flag to enable or disable local storage of data.
     */
    bool store_locally = false;

    /**
     * Flag to enable or disable local encryption of data.
     * The store_locally variable needs to be set to true is this is set to true.
     */
    bool encrypt_locally;

    /**
     * The key used to encrypt locally some piece of data.
     * It is required only if the local encryption is set to true.
     */
    uint8_t *key;

    std::ofstream file;

    /**
     * Starts the bandwidth test.
     * @param wifi_comms Used to send and receive message
     * when assessing the performance.
     */
    static void start_bandwidth_test(WifiComms *wifi_comms);
};


#endif
