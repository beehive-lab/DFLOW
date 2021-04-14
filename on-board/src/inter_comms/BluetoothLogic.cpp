#include "BluetoothLogic.h"
#include "ctime"
#include <cstring>
#include <vector>
#include "pipes.hpp"
#include "thread"
#include "can_module.hpp"
#include "config.hpp"

using namespace std;

BluetoothLogic::BluetoothLogic(OnBoardDataInterface* data_interface) {
    BluetoothLogic::data_interface = data_interface;
}

void BluetoothLogic::send_profiling_data(BluetoothComms bluetoothComms) {

    while (true) {
        bool sending_data = false;
        char *response = new char[BUFFER_SIZE];

        time_t time_of_batch = data_interface->getSignalBatch();

        if (type_of_comms == 4) {
            strcat(response, "stream-profiling-module");
        }
        if (currently_streaming[CPU_USAGE_PIPE]) {
            float value = data_interface->getFloatData(CPU_USAGE_PIPE);
            if (value != -1) {
                sending_data = true;
                strcat(response, ":CPU_USAGE:");
                char value_char[256];
                sprintf(value_char, "%G", value);
                strcat(response, value_char);
                strcat(response, ":");
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", time_of_batch);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[CPU_TEMPERATURE_PIPE]) {
            int value = data_interface->getIntegerData(CPU_TEMPERATURE_PIPE);
            if (value != -1) {
                sending_data = true;
                strcat(response, ":CPU_TEMPERATURE:");
                char value_char[256];
                sprintf(value_char, "%d", value);
                strcat(response, value_char);
                strcat(response, ":");
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", time_of_batch);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[CPU_FREQUENCY_PIPE]) {
            int value = data_interface->getIntegerData(CPU_FREQUENCY_PIPE);
            if (value != -1) {
                sending_data = true;
                strcat(response, ":CPU_FREQUENCY:");
                char value_char[256];
                sprintf(value_char, "%d", value);
                strcat(response, value_char);
                strcat(response, ":");
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", time_of_batch);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[MEMORY_USAGE_PIPE]) {
            int value = data_interface->getIntegerData(MEMORY_USAGE_PIPE);
            if (value != -1) {
                sending_data = true;
                strcat(response, ":MEMORY_USAGE:");
                char value_char[256];
                sprintf(value_char, "%d", value);
                strcat(response, value_char);
                strcat(response, ":");
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", time_of_batch);
                strcat(response, time_stamp_char);
            }
        }

        if (stopping) {
            return;
        }

        if (sending_data) {
            bluetoothComms.send(response);
        }

        this_thread::sleep_for(chrono::milliseconds(1000));
    }
}

void BluetoothLogic::receive_loop(BluetoothComms *bluetooth_comms, char receive_buffer[BUFFER_SIZE]) {

    while (true) {
        memset(receive_buffer, 0, BUFFER_SIZE);
        int bytes_received = bluetooth_comms->receive(receive_buffer);

        int abs_mode = -1, tc_mode = -1, tr_mode = -1;

        if (bytes_received <= 0) {
            stopping = true;
            fill(begin(currently_streaming), end(currently_streaming), false);
            return;
        }

        char* token = strtok(receive_buffer, ":");

        while (token != nullptr) {
            if (strcmp(token, "exit-application") == 0) {
                stopping = true;
                exit_application = true;
            } else if (strcmp(token, "stream-profiling-data") == 0) {
                type_of_comms = 4;
            } else if (strcmp(token, "encryption") == 0) {
                type_of_comms = 2;
            } else if (strcmp(token, "configure-pipe") == 0) {
                type_of_comms = 1;
            }
            else if (strcmp(token, "stream-bike-sensor-data") == 0) {
                type_of_comms = 0;
            } else if (strcmp(token, "start") == 0) {
                starting = true;
            } else if (strcmp(token, "stop") == 0) {
                starting = false;
            } else if (strcmp(token, "all") == 0) {
                if (type_of_comms == 0) {
                    if (starting) {
                        fill(begin(currently_streaming), end(currently_streaming) - 4, true);
                    } else {
                        fill(begin(currently_streaming), end(currently_streaming) - 4, false);
                    }
                } else if (type_of_comms == 4) {
                    if (starting) {
                        fill(end(currently_streaming) - 4, end(currently_streaming), true);
                    } else {
                        fill(end(currently_streaming) - 4, end(currently_streaming), false);
                    }
                }
            } else if (strcmp(token, "AIR_TEMPERATURE") == 0) {
                currently_streaming[AIR_TEMPERATURE_PIPE] = starting;
            } else if (strcmp(token, "THROTTLE_POSITION") == 0) {
                currently_streaming[THROTTLE_POSITION_PIPE] = starting;
            } else if (strcmp(token, "TYRE_PRESSURE_FRONT") == 0) {
                currently_streaming[TYRE_PRESSURE_FRONT_PIPE] = starting;
            } else if (strcmp(token, "TYRE_PRESSURE_REAR") == 0) {
                currently_streaming[TYRE_PRESSURE_REAR_PIPE] = starting;
            } else if (strcmp(token, "MOTORCYCLE_SPEED") == 0) {
                currently_streaming[MOTORCYCLE_SPEED_PIPE] = starting;
            } else if (strcmp(token, "REAR_WHEEL_SPEED") == 0) {
                currently_streaming[REAR_WHEEL_SPEED_PIPE] = starting;
            } else if (strcmp(token, "FRONT_WHEEL_SPEED") == 0) {
                currently_streaming[FRONT_WHEEL_SPEED_PIPE] = starting;
            } else if (strcmp(token, "BRAKE_REAR_ACTIVE") == 0) {
                currently_streaming[BRAKE_REAR_ACTIVE_PIPE] = starting;
            } else if (strcmp(token, "BRAKE_FRONT_ACTIVE") == 0) {
                currently_streaming[BRAKE_FRONT_ACTIVE_PIPE] = starting;
            } else if (strcmp(token, "ABS_MODE") == 0) {
                currently_streaming[ABS_MODE_PIPE] = starting;
            } else if (strcmp(token, "TC_MODE") == 0) {
                currently_streaming[TC_MODE_PIPE] = starting;
            } else if (strcmp(token, "THROTTLE_RESPONSE_MODE") == 0) {
                currently_streaming[THROTTLE_RESPONSE_MODE_PIPE] = starting;
            } else if (strcmp(token, "LEAN_ANGLE") == 0) {
                currently_streaming[LEAN_ANGLE_PIPE] = starting;
            } else if (strcmp(token, "BATTERY_VOLTAGE") == 0) {
                currently_streaming[BATTERY_VOLTAGE_PIPE] = starting;
            } else if (strcmp(token, "OIL_PRESSURE") == 0) {
                currently_streaming[OIL_PRESSURE_PIPE] = starting;
            } else if (strcmp(token, "GEAR_POSITION") == 0) {
                currently_streaming[GEAR_POSITION_PIPE] = starting;
            } else if (strcmp(token, "WATER_TEMPERATURE") == 0) {
                currently_streaming[WATER_TEMPERATURE_PIPE] = starting;
            } else if (strcmp(token, "ENGINE_SPEED") == 0) {
                currently_streaming[ENGINE_SPEED_PIPE] = starting;
            } else if (strcmp(token, "ACCELERATION_X") == 0) {
                currently_streaming[ACCELERATION_X_PIPE] = starting;
            } else if (strcmp(token, "ACCELERATION_Y") == 0) {
                currently_streaming[ACCELERATION_Y_PIPE] = starting;
            } else if (strcmp(token, "ACCELERATION_Z") == 0) {
                currently_streaming[ACCELERATION_Z_PIPE] = starting;
            } else if (strcmp(token, "CPU_USAGE") == 0) {
                currently_streaming[CPU_USAGE_PIPE] = starting;
            } else if (strcmp(token, "CPU_TEMPERATURE") == 0) {
                currently_streaming[CPU_TEMPERATURE_PIPE] = starting;
            } else if (strcmp(token, "CPU_FREQUENCY") == 0) {
                currently_streaming[CPU_FREQUENCY_PIPE] = starting;
            } else if (strcmp(token, "MEMORY_USAGE") == 0) {
                currently_streaming[MEMORY_USAGE_PIPE] = starting;
            } else if (type_of_comms == 1) {
                if (abs_mode == -1) {
                    abs_mode = stoi(token);
                } else if (tc_mode == -1) {
                    tc_mode = stoi(token);
                } else if (tr_mode == -1) {
                    tr_mode = stoi(token);
                    data_interface->sendConfigMessage(abs_mode,tc_mode,tr_mode);
                }
            } else if (type_of_comms == 2) {
                if (strcmp(token, "on") == 0) {
                    bluetooth_comms->set_encryption(true);
                } else if (strcmp(token, "off") == 0) {
                    bluetooth_comms->set_encryption(false);
                }
            }

            token = strtok(nullptr, ":");
        }
    }
}


void BluetoothLogic::send_bike_metrics(BluetoothComms bluetooth_comms) {

    while (true) {

        bool sending_data = false;
        char *response = new char[BUFFER_SIZE];

        time_t time_of_batch = data_interface->getSignalBatch();

        if (type_of_comms == 0) {
            strcat(response, "stream-bike-sensor-data");
        }
        if (currently_streaming[AIR_TEMPERATURE_PIPE]) {
            float value = data_interface->getFloatData(AIR_TEMPERATURE_PIPE);
            if (value != -1) {
                sending_data = true;
                strcat(response, ":AIR_TEMPERATURE:");
                char value_char[256];
                sprintf(value_char, "%G", value);
                strcat(response, value_char);
                strcat(response, ":");
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", time_of_batch);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[THROTTLE_POSITION_PIPE]) {
            int value = data_interface->getIntegerData(THROTTLE_POSITION_PIPE);
            if (value != -1) {
                sending_data = true;
                strcat(response, ":THROTTLE_POSITION:");
                char value_char[256];
                sprintf(value_char, "%i", value);
                strcat(response, value_char);
                strcat(response, ":");
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", time_of_batch);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[TYRE_PRESSURE_FRONT_PIPE]) {
            float value = data_interface->getFloatData(TYRE_PRESSURE_FRONT_PIPE);
            if (value != -1) {
                sending_data = true;
                strcat(response, ":TYRE_PRESSURE_FRONT:");
                char value_char[256];
                sprintf(value_char, "%G", value);
                strcat(response, value_char);
                strcat(response, ":");
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", time_of_batch);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[TYRE_PRESSURE_REAR_PIPE]) {
            float value = data_interface->getFloatData(TYRE_PRESSURE_REAR_PIPE);
            if (value != -1) {
                sending_data = true;
                strcat(response, ":TYRE_PRESSURE_REAR:");
                char value_char[256];
                sprintf(value_char, "%G", value);
                strcat(response, value_char);
                strcat(response, ":");
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", time_of_batch);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[MOTORCYCLE_SPEED_PIPE]) {
            int value = data_interface->getIntegerData(MOTORCYCLE_SPEED_PIPE);
            if (value != -1) {
                sending_data = true;
                strcat(response, ":MOTORCYCLE_SPEED:");
                char value_char[256];
                sprintf(value_char, "%i", value);
                strcat(response, value_char);
                strcat(response, ":");
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", time_of_batch);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[REAR_WHEEL_SPEED_PIPE]) {
            int value = data_interface->getIntegerData(REAR_WHEEL_SPEED_PIPE);
            if (value != -1) {
                sending_data = true;
                strcat(response, ":REAR_WHEEL_SPEED:");
                char value_char[256];
                sprintf(value_char, "%i", value);
                strcat(response, value_char);
                strcat(response, ":");
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", time_of_batch);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[FRONT_WHEEL_SPEED_PIPE]) {
            int value = data_interface->getIntegerData(FRONT_WHEEL_SPEED_PIPE);
            if (value != -1) {
                sending_data = true;
                strcat(response, ":FRONT_WHEEL_SPEED:");
                char value_char[256];
                sprintf(value_char, "%i", value);
                strcat(response, value_char);
                strcat(response, ":");
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", time_of_batch);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[BRAKE_REAR_ACTIVE_PIPE]) {
            int value = data_interface->getIntegerData(BRAKE_REAR_ACTIVE_PIPE);
            if (value != -1) {
                sending_data = true;
                strcat(response, ":BRAKE_REAR_ACTIVE:");
                char value_char[256];
                sprintf(value_char, "%i", value);
                strcat(response, value_char);
                strcat(response, ":");
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", time_of_batch);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[BRAKE_FRONT_ACTIVE_PIPE]) {
            int value = data_interface->getIntegerData(BRAKE_FRONT_ACTIVE_PIPE);
            if (value != -1) {
                sending_data = true;
                strcat(response, ":BRAKE_FRONT_ACTIVE:");
                char value_char[256];
                sprintf(value_char, "%i", value);
                strcat(response, value_char);
                strcat(response, ":");
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", time_of_batch);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[ABS_MODE_PIPE]) {
            int value = data_interface->getIntegerData(ABS_MODE_PIPE);
            if (value != -1) {
                sending_data = true;
                strcat(response, ":ABS_MODE:");
                char value_char[256];
                sprintf(value_char, "%i", value);
                strcat(response, value_char);
                strcat(response, ":");
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", time_of_batch);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[TC_MODE_PIPE]) {
            int value = data_interface->getIntegerData(TC_MODE_PIPE);
            if (value != -1) {
                sending_data = true;
                strcat(response, ":TC_MODE:");
                char value_char[256];
                sprintf(value_char, "%i", value);
                strcat(response, value_char);
                strcat(response, ":");
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", time_of_batch);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[THROTTLE_RESPONSE_MODE_PIPE]) {
            int value = data_interface->getIntegerData(THROTTLE_POSITION_PIPE);
            if (value != -1) {
                sending_data = true;
                strcat(response, ":THROTTLE_RESPONSE_MODE:");
                char value_char[256];
                sprintf(value_char, "%i", value);
                strcat(response, value_char);
                strcat(response, ":");
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", time_of_batch);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[LEAN_ANGLE_PIPE]) {
            float value = data_interface->getFloatData(LEAN_ANGLE_PIPE);
            if (value != -1) {
                sending_data = true;
                strcat(response, ":LEAN_ANGLE:");
                char value_char[256];
                sprintf(value_char, "%G", value);
                strcat(response, value_char);
                strcat(response, ":");
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", time_of_batch);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[BATTERY_VOLTAGE_PIPE]) {
            float value = data_interface->getFloatData(BATTERY_VOLTAGE_PIPE);
            if (value != -1) {
                sending_data = true;
                strcat(response, ":BATTERY_VOLTAGE:");
                char value_char[256];
                sprintf(value_char, "%G", value);
                strcat(response, value_char);
                strcat(response, ":");
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", time_of_batch);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[OIL_PRESSURE_PIPE]) {
            float value = data_interface->getFloatData(OIL_PRESSURE_PIPE);
            if (value != -1) {
                sending_data = true;
                strcat(response, ":OIL_PRESSURE:");
                char value_char[256];
                sprintf(value_char, "%G", value);
                strcat(response, value_char);
                strcat(response, ":");
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", time_of_batch);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[GEAR_POSITION_PIPE]) {
            int value = data_interface->getIntegerData(GEAR_POSITION_PIPE);
            if (value != -1) {
                sending_data = true;
                strcat(response, ":GEAR_POSITION:");
                char value_char[256];
                sprintf(value_char, "%i", value);
                strcat(response, value_char);
                strcat(response, ":");
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", time_of_batch);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[WATER_TEMPERATURE_PIPE]) {
            float value = data_interface->getFloatData(WATER_TEMPERATURE_PIPE);
            if (value != -1) {
                sending_data = true;
                strcat(response, ":WATER_TEMPERATURE:");
                char value_char[256];
                sprintf(value_char, "%G", value);
                strcat(response, value_char);
                strcat(response, ":");
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", time_of_batch);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[ENGINE_SPEED_PIPE]) {
            int value = data_interface->getIntegerData(ENGINE_SPEED_PIPE);
            if (value != -1) {
                sending_data = true;
                strcat(response, ":ENGINE_SPEED:");
                char value_char[256];
                sprintf(value_char, "%i", value);
                strcat(response, value_char);
                strcat(response, ":");
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", time_of_batch);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[ACCELERATION_X_PIPE]) {
            float value = data_interface->getFloatData(ACCELERATION_X_PIPE);
            if (value != -1) {
                sending_data = true;
                strcat(response, ":ACCELERATION_X:");
                char value_char[256];
                sprintf(value_char, "%G", value);
                strcat(response, value_char);
                strcat(response, ":");
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", time_of_batch);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[ACCELERATION_Y_PIPE]) {
            float value = data_interface->getFloatData(ACCELERATION_Y_PIPE);
            if (value != -1) {
                sending_data = true;
                strcat(response, ":ACCELERATION_Y:");
                char value_char[256];
                sprintf(value_char, "%G", value);
                strcat(response, value_char);
                strcat(response, ":");
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", time_of_batch);
                strcat(response, time_stamp_char);
            }
        }
        if (currently_streaming[ACCELERATION_Z_PIPE]) {
            float value = data_interface->getFloatData(ACCELERATION_Z_PIPE);
            if (value != -1) {
                sending_data = true;
                strcat(response, ":ACCELERATION_Z:");
                char value_char[256];
                sprintf(value_char, "%G", value);
                strcat(response, value_char);
                strcat(response, ":");
                char time_stamp_char[256];
                sprintf(time_stamp_char, "%ld", time_of_batch);
                strcat(response, time_stamp_char);
            }
        }
        if (stopping) {
            return;
        }

        if (sending_data) {
            bluetooth_comms.send(response);
        }

        this_thread::sleep_for(chrono::milliseconds(1000));
    }
}