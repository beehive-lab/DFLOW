#include <vector>


#ifndef PIPES_H
#define PIPES_H

//global variables defining pipe indexes
extern const int AIR_TEMPERATURE_PIPE;
extern const int THROTTLE_POSITION_PIPE;
extern const int TYRE_PRESSURE_FRONT_PIPE;
extern const int TYRE_PRESSURE_REAR_PIPE;
extern const int MOTORCYCLE_SPEED_PIPE;
extern const int REAR_WHEEL_SPEED_PIPE;
extern const int FRONT_WHEEL_SPEED_PIPE;
extern const int BRAKE_REAR_ACTIVE_PIPE;
extern const int BRAKE_FRONT_ACTIVE_PIPE;
extern const int ABS_MODE_PIPE;
extern const int TC_MODE_PIPE;
extern const int THROTTLE_RESPONSE_MODE_PIPE;
extern const int LEAN_ANGLE_PIPE;
extern const int BATTERY_VOLTAGE_PIPE;
extern const int OIL_PRESSURE_PIPE;
extern const int GEAR_POSITION_PIPE;
extern const int WATER_TEMPERATURE_PIPE;
extern const int ENGINE_SPEED_PIPE;
extern const int ACCELERATION_X_PIPE;
extern const int ACCELERATION_Y_PIPE;
extern const int ACCELERATION_Z_PIPE;
extern const int TIMESTAMP_PIPE;

extern const int INTAKE_MESSAGE_PIPE;
extern const int ABS_MESSAGE_PIPE;
extern const int TPM_MESSAGE_PIPE;
extern const int ENGINE_MESSAGE_PIPE;
extern const int CONFIG_MESSAGE_PIPE;
extern const int IMU_MESSAGE_PIPE;
extern const int ACCELEROMETER_MESSAGE_PIPE;


extern const int READ;
extern const int WRITE;

struct Pipes
{
    int rdwr[2];
};

#endif