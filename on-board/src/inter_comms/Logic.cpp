#include "Logic.h"
#include "WifiComms.h"
#include "BluetoothComms.h"
#include "ctime"
#include <iostream>
#include <cstring>
#include <vector>
#include <unistd.h>
#include "pipes.hpp"
#include "thread"

using namespace std;

void Logic::read_and_send(WifiComms wifiComms) {

    bool sending_data = true;

    while (sending_data) {

        sending_data = false;
        char *response = new char[BUFFER_SIZE];

        if (type_of_comms == 0) {
            strcat(response, "stream-bike-sensor-data");
        }
        if (currently_streaming[AIR_TEMPERATURE_PIPE]) {
            float value;
            read(Logic::processed_pipes_vector[AIR_TEMPERATURE_PIPE].rdwr[READ], &value, sizeof(float));
            if (value != -1) {
                sending_data = true;
                strcat(response, ":AIR_TEMPERATURE:");
                char value_char[256];
                sprintf(value_char, "%G", value);
                strcat(response, value_char);
                strcat(response, ":");
                time_t timestamp_now;
                read(Logic::processed_pipes_vector[TIMESTAMP_PIPE].rdwr[READ], &timestamp_now, sizeof(time_t));
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", timestamp_now);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[THROTTLE_POSITION_PIPE]) {
            int value;
            read(Logic::processed_pipes_vector[THROTTLE_POSITION_PIPE].rdwr[READ], &value, sizeof(int));
            if (value != -1) {
                sending_data = true;
                strcat(response, ":THROTTLE_POSITION:");
                char value_char[256];
                sprintf(value_char, "%i", value);
                strcat(response, value_char);
                strcat(response, ":");
                time_t timestamp_now;
                read(Logic::processed_pipes_vector[TIMESTAMP_PIPE].rdwr[READ], &timestamp_now, sizeof(time_t));
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", timestamp_now);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[TYRE_PRESSURE_FRONT_PIPE]) {
            float value;
            read(Logic::processed_pipes_vector[TYRE_PRESSURE_FRONT_PIPE].rdwr[READ], &value, sizeof(float));
            if (value != -1) {
                sending_data = true;
                strcat(response, ":TYRE_PRESSURE_FRONT:");
                char value_char[256];
                sprintf(value_char, "%G", value);
                strcat(response, value_char);
                strcat(response, ":");
                time_t timestamp_now;
                read(Logic::processed_pipes_vector[TIMESTAMP_PIPE].rdwr[READ], &timestamp_now, sizeof(time_t));
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", timestamp_now);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[TYRE_PRESSURE_REAR_PIPE]) {
            float value;
            read(Logic::processed_pipes_vector[TYRE_PRESSURE_REAR_PIPE].rdwr[READ], &value, sizeof(float));
            if (value != -1) {
                sending_data = true;
                strcat(response, ":TYRE_PRESSURE_REAR:");
                char value_char[256];
                sprintf(value_char, "%G", value);
                strcat(response, value_char);
                strcat(response, ":");
                time_t timestamp_now;
                read(Logic::processed_pipes_vector[TIMESTAMP_PIPE].rdwr[READ], &timestamp_now, sizeof(time_t));
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", timestamp_now);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[MOTORCYCLE_SPEED_PIPE]) {
            int value;
            read(Logic::processed_pipes_vector[MOTORCYCLE_SPEED_PIPE].rdwr[READ], &value, sizeof(int));
            if (value != -1) {
                sending_data = true;
                strcat(response, ":MOTORCYCLE_SPEED:");
                char value_char[256];
                sprintf(value_char, "%i", value);
                strcat(response, value_char);
                strcat(response, ":");
                time_t timestamp_now;
                read(Logic::processed_pipes_vector[TIMESTAMP_PIPE].rdwr[READ], &timestamp_now, sizeof(time_t));
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", timestamp_now);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[REAR_WHEEL_SPEED_PIPE]) {
            int value;
            read(Logic::processed_pipes_vector[REAR_WHEEL_SPEED_PIPE].rdwr[READ], &value, sizeof(int));
            if (value != -1) {
                sending_data = true;
                strcat(response, ":REAR_WHEEL_SPEED:");
                char value_char[256];
                sprintf(value_char, "%i", value);
                strcat(response, value_char);
                strcat(response, ":");
                time_t timestamp_now;
                read(Logic::processed_pipes_vector[TIMESTAMP_PIPE].rdwr[READ], &timestamp_now, sizeof(time_t));
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", timestamp_now);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[FRONT_WHEEL_SPEED_PIPE]) {
            int value;
            read(Logic::processed_pipes_vector[FRONT_WHEEL_SPEED_PIPE].rdwr[READ], &value, sizeof(int));
            if (value != -1) {
                sending_data = true;
                strcat(response, ":FRONT_WHEEL_SPEED:");
                char value_char[256];
                sprintf(value_char, "%i", value);
                strcat(response, value_char);
                strcat(response, ":");
                time_t timestamp_now;
                read(Logic::processed_pipes_vector[TIMESTAMP_PIPE].rdwr[READ], &timestamp_now, sizeof(time_t));
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", timestamp_now);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[BRAKE_REAR_ACTIVE_PIPE]) {
            int value;
            read(Logic::processed_pipes_vector[BRAKE_REAR_ACTIVE_PIPE].rdwr[READ], &value, sizeof(int));
            if (value != -1) {
                sending_data = true;
                strcat(response, ":BRAKE_REAR_ACTIVE:");
                char value_char[256];
                sprintf(value_char, "%i", value);
                strcat(response, value_char);
                strcat(response, ":");
                time_t timestamp_now;
                read(Logic::processed_pipes_vector[TIMESTAMP_PIPE].rdwr[READ], &timestamp_now, sizeof(time_t));
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", timestamp_now);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[BRAKE_FRONT_ACTIVE_PIPE]) {
            int value;
            read(Logic::processed_pipes_vector[BRAKE_FRONT_ACTIVE_PIPE].rdwr[READ], &value, sizeof(int));
            if (value != -1) {
                sending_data = true;
                strcat(response, ":BRAKE_FRONT_ACTIVE:");
                char value_char[256];
                sprintf(value_char, "%i", value);
                strcat(response, value_char);
                strcat(response, ":");
                time_t timestamp_now;
                read(Logic::processed_pipes_vector[TIMESTAMP_PIPE].rdwr[READ], &timestamp_now, sizeof(time_t));
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", timestamp_now);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[ABS_MODE_PIPE]) {
            int value;
            read(Logic::processed_pipes_vector[ABS_MODE_PIPE].rdwr[READ], &value, sizeof(int));
            if (value != -1) {
                sending_data = true;
                strcat(response, ":ABS_MODE:");
                char value_char[256];
                sprintf(value_char, "%i", value);
                strcat(response, value_char);
                strcat(response, ":");
                time_t timestamp_now;
                read(Logic::processed_pipes_vector[TIMESTAMP_PIPE].rdwr[READ], &timestamp_now, sizeof(time_t));
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", timestamp_now);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[TC_MODE_PIPE]) {
            int value;
            read(Logic::processed_pipes_vector[TC_MODE_PIPE].rdwr[READ], &value, sizeof(int));
            if (value != -1) {
                sending_data = true;
                strcat(response, ":TC_MODE:");
                char value_char[256];
                sprintf(value_char, "%i", value);
                strcat(response, value_char);
                strcat(response, ":");
                time_t timestamp_now;
                read(Logic::processed_pipes_vector[TIMESTAMP_PIPE].rdwr[READ], &timestamp_now, sizeof(time_t));
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", timestamp_now);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[THROTTLE_RESPONSE_MODE_PIPE]) {
            int value;
            read(Logic::processed_pipes_vector[THROTTLE_RESPONSE_MODE_PIPE].rdwr[READ], &value, sizeof(int));
            if (value != -1) {
                sending_data = true;
                strcat(response, ":THROTTLE_RESPONSE_MODE:");
                char value_char[256];
                sprintf(value_char, "%i", value);
                strcat(response, value_char);
                strcat(response, ":");
                time_t timestamp_now;
                read(Logic::processed_pipes_vector[TIMESTAMP_PIPE].rdwr[READ], &timestamp_now, sizeof(time_t));
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", timestamp_now);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[LEAN_ANGLE_PIPE]) {
            float value;
            read(Logic::processed_pipes_vector[LEAN_ANGLE_PIPE].rdwr[READ], &value, sizeof(float));
            if (value != -1) {
                sending_data = true;
                strcat(response, ":LEAN_ANGLE:");
                char value_char[256];
                sprintf(value_char, "%G", value);
                strcat(response, value_char);
                strcat(response, ":");
                time_t timestamp_now;
                read(Logic::processed_pipes_vector[TIMESTAMP_PIPE].rdwr[READ], &timestamp_now, sizeof(time_t));
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", timestamp_now);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[BATTERY_VOLTAGE_PIPE]) {
            float value;
            read(Logic::processed_pipes_vector[BATTERY_VOLTAGE_PIPE].rdwr[READ], &value, sizeof(float));
            if (value != -1) {
                sending_data = true;
                strcat(response, ":BATTERY_VOLTAGE:");
                char value_char[256];
                sprintf(value_char, "%G", value);
                strcat(response, value_char);
                strcat(response, ":");
                time_t timestamp_now;
                read(Logic::processed_pipes_vector[TIMESTAMP_PIPE].rdwr[READ], &timestamp_now, sizeof(time_t));
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", timestamp_now);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[OIL_PRESSURE_PIPE]) {
            float value;
            read(Logic::processed_pipes_vector[OIL_PRESSURE_PIPE].rdwr[READ], &value, sizeof(float));
            if (value != -1) {
                sending_data = true;
                strcat(response, ":OIL_PRESSURE:");
                char value_char[256];
                sprintf(value_char, "%G", value);
                strcat(response, value_char);
                strcat(response, ":");
                time_t timestamp_now;
                read(Logic::processed_pipes_vector[TIMESTAMP_PIPE].rdwr[READ], &timestamp_now, sizeof(time_t));
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", timestamp_now);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[GEAR_POSITION_PIPE]) {
            int value;
            read(Logic::processed_pipes_vector[GEAR_POSITION_PIPE].rdwr[READ], &value, sizeof(int));
            if (value != -1) {
                sending_data = true;
                strcat(response, ":GEAR_POSITION:");
                char value_char[256];
                sprintf(value_char, "%i", value);
                strcat(response, value_char);
                strcat(response, ":");
                time_t timestamp_now;
                read(Logic::processed_pipes_vector[TIMESTAMP_PIPE].rdwr[READ], &timestamp_now, sizeof(time_t));
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", timestamp_now);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[WATER_TEMPERATURE_PIPE]) {
            float value;
            read(Logic::processed_pipes_vector[WATER_TEMPERATURE_PIPE].rdwr[READ], &value, sizeof(float));
            if (value != -1) {
                sending_data = true;
                strcat(response, ":WATER_TEMPERATURE:");
                char value_char[256];
                sprintf(value_char, "%G", value);
                strcat(response, value_char);
                strcat(response, ":");
                time_t timestamp_now;
                read(Logic::processed_pipes_vector[TIMESTAMP_PIPE].rdwr[READ], &timestamp_now, sizeof(time_t));
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", timestamp_now);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[ENGINE_SPEED_PIPE]) {
            int value;
            read(Logic::processed_pipes_vector[ENGINE_SPEED_PIPE].rdwr[READ], &value, sizeof(int));
            if (value != -1) {
                sending_data = true;
                strcat(response, ":ENGINE_SPEED:");
                char value_char[256];
                sprintf(value_char, "%i", value);
                strcat(response, value_char);
                strcat(response, ":");
                time_t timestamp_now;
                read(Logic::processed_pipes_vector[TIMESTAMP_PIPE].rdwr[READ], &timestamp_now, sizeof(time_t));
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", timestamp_now);
                strcat(response, time_stamp_char);
            }
        }

        if (sending_data) {
            wifiComms.send(response);
        }

        this_thread::sleep_for(chrono::milliseconds(1000));
    }

}

void Logic::receive_loop(WifiComms wifiComms, char receive_buffer[BUFFER_SIZE]) {

    // This part doesn't work. I need to create a thread separately and this can't
    // be done from a non-static method.

    // thread values_thread(read_and_send, wifiComms);

    while (true) {
        memset(receive_buffer, 0, BUFFER_SIZE);
        wifiComms.receive(receive_buffer);

        char* token = strtok(receive_buffer, ":");

        while (token != nullptr) {

            if (strcmp(token, "stream-bike-sensor-data") == 0) {
                type_of_comms = 0;
            } else if (strcmp(token, "start") == 0) {
                starting = true;
            } else if (strcmp(token, "stop") == 0) {
                starting = false;
            } else if (strcmp(token, "air_temperature") == 0) {
                currently_streaming[AIR_TEMPERATURE_PIPE] = starting;
            } else if (strcmp(token, "throttle_position") == 0) {
                currently_streaming[THROTTLE_POSITION_PIPE] = starting;
            } else if (strcmp(token, "tyre_pressure_front") == 0) {
                currently_streaming[TYRE_PRESSURE_FRONT_PIPE] = starting;
            } else if (strcmp(token, "tyre_pressure_rear") == 0) {
                currently_streaming[TYRE_PRESSURE_REAR_PIPE] = starting;
            } else if (strcmp(token, "motorcycle_speed") == 0) {
                currently_streaming[MOTORCYCLE_SPEED_PIPE] = starting;
            } else if (strcmp(token, "rear_wheel_speed") == 0) {
                currently_streaming[REAR_WHEEL_SPEED_PIPE] = starting;
            } else if (strcmp(token, "front_wheel_speed") == 0) {
                currently_streaming[FRONT_WHEEL_SPEED_PIPE] = starting;
            } else if (strcmp(token, "brake_rear_active") == 0) {
                currently_streaming[BRAKE_REAR_ACTIVE_PIPE] = starting;
            } else if (strcmp(token, "brake_front_active") == 0) {
                currently_streaming[BRAKE_FRONT_ACTIVE_PIPE] = starting;
            } else if (strcmp(token, "abs_mode") == 0) {
                currently_streaming[ABS_MODE_PIPE] = starting;
            } else if (strcmp(token, "tc_mode") == 0) {
                currently_streaming[TC_MODE_PIPE] = starting;
            } else if (strcmp(token, "throttle_response_mode") == 0) {
                currently_streaming[THROTTLE_RESPONSE_MODE_PIPE] = starting;
            } else if (strcmp(token, "lean_angle") == 0) {
                currently_streaming[LEAN_ANGLE_PIPE] = starting;
            } else if (strcmp(token, "battery_voltage") == 0) {
                currently_streaming[BATTERY_VOLTAGE_PIPE] = starting;
            } else if (strcmp(token, "oil_pressure") == 0) {
                currently_streaming[OIL_PRESSURE_PIPE] = starting;
            } else if (strcmp(token, "gear_position") == 0) {
                currently_streaming[GEAR_POSITION_PIPE] = starting;
            } else if (strcmp(token, "water_temperature") == 0) {
                currently_streaming[WATER_TEMPERATURE_PIPE] = starting;
            } else if (strcmp(token, "engine_speed") == 0) {
                currently_streaming[ENGINE_SPEED_PIPE] = starting;
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

    Logic::receive_loop(wifi_comms, receive_buffer);
}

void Logic::Bluetooth_logic(bool logging, int channel) {

}

Logic::Logic(vector<Pipes> processed_pipes_vector_init) {
    Logic::processed_pipes_vector = processed_pipes_vector_init;
}
