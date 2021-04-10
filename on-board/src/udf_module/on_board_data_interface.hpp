#include <ctime>
#include <fcntl.h>
#include "pipes.hpp"
#include <vector>
#include <unistd.h>
#include <climits>
#include <cfloat>

#ifndef ON_BOARD_DATA_INTERFACE_H
#define ON_BOARD_DATA_INTERFACE_H

class OnBoardDataInterface{
    public:
        OnBoardDataInterface(std::vector<Pipes>);
        time_t getSignalBatch();
        float getFloatData(int);
        int getIntegerData(int);
        bool getBooleanData(int);
    private:
        float air_temperature,tyre_pressure_front,tyre_pressure_rear,lean_angle, battery_voltage,oil_pressure,water_temperature,acceleration_x,acceleration_y,acceleration_z;
        int throttle_position,motorcycle_speed,rear_wheel_speed,front_wheel_speed,rear_brake_active,front_brake_active,abs_mode,tc_mode,throttle_response_mode,gear_position,engine_speed;
        time_t time_received;

        std::vector<Pipes> processed_pipes_vector;
};

#endif