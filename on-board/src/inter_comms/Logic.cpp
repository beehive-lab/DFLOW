#include "Logic.h"
#include "WifiComms.h"
#include "BluetoothComms.h"
#include <thread>
#include <iostream>
#include <cstring>
#include <utility>
#include <vector>
#include "pipes.hpp"
#include "unistd.h"

using namespace std;

void Logic::receive_loop(WifiComms wifiComms, char receive_buffer[BUFFER_SIZE]) {

    memset(receive_buffer, 0, BUFFER_SIZE);

    while (true) {
        wifiComms.receive(receive_buffer);
        cout<<receive_buffer<<endl;

        char* token = strtok(receive_buffer, ":");

        char *response = new char[BUFFER_SIZE];

        while (token != nullptr) {
            cout<<token<<endl;

            if (strcmp(token, "data") == 0) {
                strcat(response, token);
            } else if (strcmp(token, "AIR_TEMPERATURE") == 0) {
                float value;
                read(processed_pipes_vector[AIR_TEMPERATURE_PIPE].rdwr[READ], &value, sizeof(float));
                strcat(response, ":AIR_TEMPERATURE:");
                char value_char[256];
                sprintf(value_char, "%G", value);
                strcat(response, value_char);
                wifiComms.send(response);
            }

            token = strtok(nullptr, ":");
        }
        return;
    }
}

void Logic::Wifi_logic(bool logging, int port) {
    WifiComms wifi_comms(logging);

    wifi_comms.establish_connection(port);

    char receive_buffer[BUFFER_SIZE];

    char send_buffer[BUFFER_SIZE];

    thread receive_data(receive_loop, wifi_comms, receive_buffer);

    receive_data.join();
}

void Logic::Bluetooth_logic(bool logging, int channel) {

}

Logic::Logic(vector<Pipes> processed_pipes_vector_init) {
    processed_pipes_vector = std::move(processed_pipes_vector_init);
}
