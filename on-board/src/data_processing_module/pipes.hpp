//Author: Radu-Tudor Andra


#ifndef PIPES_H
#define PIPES_H

extern const int SIGNAL_NUMBER;
extern const int MESSAGE_NUMBER;

//global variables defining pipe indexes
extern const int AIR_TEMPERATURE;
extern const int THROTTLE_POSITION;
extern const int TYRE_PRESSURE_FRONT;
extern const int TYRE_PRESSURE_REAR;
extern const int MOTORCYCLE_SPEED;
extern const int REAR_WHEEL_SPEED;
extern const int FRONT_WHEEL_SPEED;
extern const int BRAKE_REAR_ACTIVE;
extern const int BRAKE_FRONT_ACTIVE;
extern const int ABS_MODE;
extern const int TC_MODE;
extern const int THROTTLE_RESPONSE_MODE;
extern const int LEAN_ANGLE;
extern const int BATTERY_VOLTAGE;
extern const int OIL_PRESSURE;
extern const int GEAR_POSITION;
extern const int WATER_TEMPERATURE;
extern const int ENGINE_SPEED;
extern const int ACCELERATION_X;
extern const int ACCELERATION_Y;
extern const int ACCELERATION_Z;
extern const int TIMESTAMP;
extern const int CPU_USAGE;
extern const int CPU_FREQUENCY;
extern const int CPU_TEMPERATURE;
extern const int MEMORY_USAGE;

extern const int INTAKE_MESSAGE;
extern const int ABS_MESSAGE;
extern const int TPM_MESSAGE;
extern const int ENGINE_MESSAGE;
extern const int CONFIG_MESSAGE;
extern const int IMU_MESSAGE;
extern const int ACCELEROMETER_MESSAGE;
extern const int PROFILING_MESSAGE;

extern const int INTEGER_TYPE;
extern const int FLOAT_TYPE;
extern const int BOOLEAN_TYPE;

extern const int READ;
extern const int WRITE;

struct Pipes
{
    int rdwr[2];
};

#endif