#include "BluetoothLogic.h"
#include "ctime"
#include <cstring>
#include <vector>
#include <algorithm>
#include "pipes.hpp"
#include "thread"
#include "can_module.hpp"
#include "config.hpp"
#include "crypto/SymmetricEncryption.h"

using namespace std;

BluetoothLogic::BluetoothLogic(OnBoardDataInterface* data_interface, bool store_locally, bool encrypt_locally, uint8_t *key) {
    BluetoothLogic::data_interface = data_interface;
    this->store_locally = store_locally;
    this->encrypt_locally = encrypt_locally;
    this->file.open("Data", ios::ate);
    if (!file) {
        cerr << "Local storage file not created";
    }
    this->key = key;
}


void BluetoothLogic::send_profiling_data(BluetoothComms bluetooth_comms) {

    while (true) {
        bool sending_data = false;
        char *response = new char[BUFFER_SIZE];

        time_t time_of_batch = data_interface->getSignalBatch();

        if (type_of_comms == 4) {
            strcat(response, "stream-profiling-module");
        }
        if (currently_streaming[CPU_USAGE]) {
            float value = data_interface->getFloatData(CPU_USAGE);
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
        if (currently_streaming[CPU_TEMPERATURE]) {
            int value = data_interface->getIntegerData(CPU_TEMPERATURE);
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
        if (currently_streaming[CPU_FREQUENCY]) {
            int value = data_interface->getIntegerData(CPU_FREQUENCY);
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
        if (currently_streaming[MEMORY_USAGE]) {
            int value = data_interface->getIntegerData(MEMORY_USAGE);
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
            bluetooth_comms.send(response);
        }

        this_thread::sleep_for(chrono::milliseconds(1000));
    }
}

void BluetoothLogic::receive_loop(BluetoothComms *bluetooth_comms, char receive_buffer[BUFFER_SIZE]) {


    bool logging_helper = bluetooth_comms->logging, store_locally_helper = store_locally;

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
            } else if (strcmp(token, "sort-benchmark") == 0) {
                type_of_comms = 5;
            } else if (strcmp(token, "stream-profiling-data") == 0) {
                type_of_comms = 4;
            } else if (strcmp(token, "start-bandwidth-test") == 0) {
                bluetooth_comms->logging = false;
                this->store_locally = false;
                type_of_comms = 3;
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
                currently_streaming[AIR_TEMPERATURE] = starting;
            } else if (strcmp(token, "THROTTLE_POSITION") == 0) {
                currently_streaming[THROTTLE_POSITION] = starting;
            } else if (strcmp(token, "TYRE_PRESSURE_FRONT") == 0) {
                currently_streaming[TYRE_PRESSURE_FRONT] = starting;
            } else if (strcmp(token, "TYRE_PRESSURE_REAR") == 0) {
                currently_streaming[TYRE_PRESSURE_REAR] = starting;
            } else if (strcmp(token, "MOTORCYCLE_SPEED") == 0) {
                currently_streaming[MOTORCYCLE_SPEED] = starting;
            } else if (strcmp(token, "REAR_WHEEL_SPEED") == 0) {
                currently_streaming[REAR_WHEEL_SPEED] = starting;
            } else if (strcmp(token, "FRONT_WHEEL_SPEED") == 0) {
                currently_streaming[FRONT_WHEEL_SPEED] = starting;
            } else if (strcmp(token, "BRAKE_REAR_ACTIVE") == 0) {
                currently_streaming[BRAKE_REAR_ACTIVE] = starting;
            } else if (strcmp(token, "BRAKE_FRONT_ACTIVE") == 0) {
                currently_streaming[BRAKE_FRONT_ACTIVE] = starting;
            } else if (strcmp(token, "ABS_MODE") == 0) {
                currently_streaming[ABS_MODE] = starting;
            } else if (strcmp(token, "TC_MODE") == 0) {
                currently_streaming[TC_MODE] = starting;
            } else if (strcmp(token, "THROTTLE_RESPONSE_MODE") == 0) {
                currently_streaming[THROTTLE_RESPONSE_MODE] = starting;
            } else if (strcmp(token, "LEAN_ANGLE") == 0) {
                currently_streaming[LEAN_ANGLE] = starting;
            } else if (strcmp(token, "BATTERY_VOLTAGE") == 0) {
                currently_streaming[BATTERY_VOLTAGE] = starting;
            } else if (strcmp(token, "OIL_PRESSURE") == 0) {
                currently_streaming[OIL_PRESSURE] = starting;
            } else if (strcmp(token, "GEAR_POSITION") == 0) {
                currently_streaming[GEAR_POSITION] = starting;
            } else if (strcmp(token, "WATER_TEMPERATURE") == 0) {
                currently_streaming[WATER_TEMPERATURE] = starting;
            } else if (strcmp(token, "ENGINE_SPEED") == 0) {
                currently_streaming[ENGINE_SPEED] = starting;
            } else if (strcmp(token, "ACCELERATION_X") == 0) {
                currently_streaming[ACCELERATION_X] = starting;
            } else if (strcmp(token, "ACCELERATION_Y") == 0) {
                currently_streaming[ACCELERATION_Y] = starting;
            } else if (strcmp(token, "ACCELERATION_Z") == 0) {
                currently_streaming[ACCELERATION_Z] = starting;
            } else if (strcmp(token, "CPU_USAGE") == 0) {
                currently_streaming[CPU_USAGE] = starting;
            } else if (strcmp(token, "CPU_TEMPERATURE") == 0) {
                currently_streaming[CPU_TEMPERATURE] = starting;
            } else if (strcmp(token, "CPU_FREQUENCY") == 0) {
                currently_streaming[CPU_FREQUENCY] = starting;
            } else if (strcmp(token, "MEMORY_USAGE") == 0) {
                currently_streaming[MEMORY_USAGE] = starting;
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
            } else if (type_of_comms == 3) {
                // Bandwidth test. Ignore message unless it is the last one
                if (strcmp(token, "bandwidth-test-request-confirm") == 0) {
                    char to_send[23] = "bandwidth-test-confirm";
                    bluetooth_comms->send(to_send);
                    start_bandwidth_test(bluetooth_comms);
                    bluetooth_comms->logging = logging_helper;
                    this->store_locally = store_locally_helper;
                }
            } else if (type_of_comms == 5) {
                unsigned long long number_of_elements = stoi(token);
                unsigned long long max_element = 1000000;
                int *array = new int[number_of_elements];

                for (unsigned long long i = 0; i < number_of_elements; i++) {
                    array[i] = rand() % max_element;
                }

                auto start = chrono::system_clock::now();
                sort(array, array + number_of_elements);
                auto end = chrono::system_clock::now();

                chrono::duration<double> time_taken = end - start;

                string temporary = "sort-benchmark-result:";
                temporary.append(to_string(time_taken.count()));
                char *to_send = new char[temporary.length() + 1];
                strcpy(to_send, temporary.c_str());

                bluetooth_comms->send(to_send);
            }

            token = strtok(nullptr, ":");
        }
    }
}

void BluetoothLogic::start_bandwidth_test(BluetoothComms *bluetooth_comms) {
    int messages = 1000;

    char message[BUFFER_SIZE + 1] = "bandwidth-test-data";

    fill(begin(message) + 19, end(message), '#');

    message[BUFFER_SIZE] = '\0';

    char receive_buffer[BUFFER_SIZE];

    char end_confirmation[] = "bandwidth-test-request-confirm";

    auto start = chrono::system_clock::now();

    for (int i = 0; i < messages; i++) {
        bluetooth_comms->send(message);
    }

    bluetooth_comms->send(end_confirmation);

    bluetooth_comms->receive(receive_buffer);

    auto end = chrono::system_clock::now();

    chrono::duration<double> send_time = end - start;

    assert(strcmp(receive_buffer, "bandwidth-test-confirm") == 0);

    double throughput = (BUFFER_SIZE * messages) / (send_time.count() * 1000);

    string temp = "bandwidth-test-result:";
    temp.append(to_string(throughput));
    char *result_to_send = new char[temp.length() + 1];
    strcpy(result_to_send, temp.c_str());

    bluetooth_comms->send(result_to_send);

    cout << "Throughput is " << throughput << " Kb/s"<<endl;
}

void BluetoothLogic::send_bike_metrics(BluetoothComms bluetooth_comms) {

    while (true) {

        bool sending_data = false;
        char *response = new char[BUFFER_SIZE];

        memset(response, 0, BUFFER_SIZE);

        time_t time_of_batch = data_interface->getSignalBatch();

        if (type_of_comms == 0) {
            strcat(response, "stream-bike-sensor-data");
        }
        if (currently_streaming[AIR_TEMPERATURE]) {
            float value = data_interface->getFloatData(AIR_TEMPERATURE);
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
        if (currently_streaming[THROTTLE_POSITION]) {
            int value = data_interface->getIntegerData(THROTTLE_POSITION);
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
        if (currently_streaming[TYRE_PRESSURE_FRONT]) {
            float value = data_interface->getFloatData(TYRE_PRESSURE_FRONT);
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
        if (currently_streaming[TYRE_PRESSURE_REAR]) {
            float value = data_interface->getFloatData(TYRE_PRESSURE_REAR);
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
        if (currently_streaming[MOTORCYCLE_SPEED]) {
            int value = data_interface->getIntegerData(MOTORCYCLE_SPEED);
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
        if (currently_streaming[REAR_WHEEL_SPEED]) {
            int value = data_interface->getIntegerData(REAR_WHEEL_SPEED);
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
        if (currently_streaming[FRONT_WHEEL_SPEED]) {
            int value = data_interface->getIntegerData(FRONT_WHEEL_SPEED);
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
        if (currently_streaming[BRAKE_REAR_ACTIVE]) {
            int value = data_interface->getIntegerData(BRAKE_REAR_ACTIVE);
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
        if (currently_streaming[BRAKE_FRONT_ACTIVE]) {
            int value = data_interface->getIntegerData(BRAKE_FRONT_ACTIVE);
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
        if (currently_streaming[ABS_MODE]) {
            int value = data_interface->getIntegerData(ABS_MODE);
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
        if (currently_streaming[TC_MODE]) {
            int value = data_interface->getIntegerData(TC_MODE);
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
        if (currently_streaming[THROTTLE_RESPONSE_MODE]) {
            int value = data_interface->getIntegerData(THROTTLE_POSITION);
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
        if (currently_streaming[LEAN_ANGLE]) {
            float value = data_interface->getFloatData(LEAN_ANGLE);
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
        if (currently_streaming[BATTERY_VOLTAGE]) {
            float value = data_interface->getFloatData(BATTERY_VOLTAGE);
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
        if (currently_streaming[OIL_PRESSURE]) {
            float value = data_interface->getFloatData(OIL_PRESSURE);
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
        if (currently_streaming[GEAR_POSITION]) {
            int value = data_interface->getIntegerData(GEAR_POSITION);
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
        if (currently_streaming[WATER_TEMPERATURE]) {
            float value = data_interface->getFloatData(WATER_TEMPERATURE);
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
        if (currently_streaming[ENGINE_SPEED]) {
            int value = data_interface->getIntegerData(ENGINE_SPEED);
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
        if (currently_streaming[ACCELERATION_X]) {
            float value = data_interface->getFloatData(ACCELERATION_X);
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
        if (currently_streaming[ACCELERATION_Y]) {
            float value = data_interface->getFloatData(ACCELERATION_Y);
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
        if (currently_streaming[ACCELERATION_Z]) {
            float value = data_interface->getFloatData(ACCELERATION_Z);
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

            if (store_locally) {
                if (!encrypt_locally) {
                    if (bluetooth_comms.logging) {
                        cout << "Storing locally (unencrypted)" << endl;
                    }

                    auto start = chrono::system_clock::now();

                    file << response << endl;

                    auto end = chrono::system_clock::now();

                    chrono::duration<double, milli> write_time = end - start;

                    if (bluetooth_comms.logging) {
                        cout << "Time taken: " << write_time.count() << " milliseconds" << endl;
                    }

                } else {
                    if (bluetooth_comms.logging) {
                        cout << "Storing locally (encrypted)" << endl;
                    }

                    auto start = chrono::system_clock::now();

                    auto *encrypted_msg = new uint8_t[1024];
                    SymmetricEncryption::encrypt(key, strlen(response), reinterpret_cast<const uint8_t *>(response), encrypted_msg);

                    file << encrypted_msg << endl;

                    auto end = chrono::system_clock::now();

                    chrono::duration<double, milli> write_time = end - start;

                    if (bluetooth_comms.logging) {
                        cout << "Time taken: " << write_time.count() << " milliseconds" << endl;
                    }
                }
            }
        }

        this_thread::sleep_for(chrono::milliseconds(1000));
    }
}