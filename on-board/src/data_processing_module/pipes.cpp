#include <pipes.hpp>

const int AIR_TEMPERATURE_PIPE      =0;
const int THROTTLE_POSITION_PIPE    =1;
const int TYRE_PRESSURE_FRONT_PIPE  =2;
const int TYRE_PRESSURE_REAR_PIPE   =3;
const int MOTORCYCLE_SPEED_PIPE     =4;
const int REAR_WHEEL_SPEED_PIPE     =5;
const int FRONT_WHEEL_SPEED_PIPE    =6;
const int BRAKE_REAR_ACTIVE_PIPE    =7;
const int BRAKE_FRONT_ACTIVE_PIPE   =8;
const int ABS_MODE_PIPE             =9;
const int TC_MODE_PIPE              =10;
const int THROTTLE_RESPONSE_MODE_PIPE=11;
const int LEAN_ANGLE_PIPE           =12;
const int BATTERY_VOLTAGE_PIPE      =13;
const int OIL_PRESSURE_PIPE         =14;
const int GEAR_POSITION_PIPE        =15;
const int WATER_TEMPERATURE_PIPE    =16;
const int ENGINE_SPEED_PIPE         =17;
const int ACCELERATION_X_PIPE       =18;
const int ACCELERATION_Y_PIPE       =19;
const int ACCELERATION_Z_PIPE       =20;
const int TIMESTAMP_PIPE            =21;

const int INTAKE_MESSAGE_PIPE       =0;
const int ABS_MESSAGE_PIPE          =1;
const int TPM_MESSAGE_PIPE          =2;
const int ENGINE_MESSAGE_PIPE       =3;
const int CONFIG_MESSAGE_PIPE       =4;
const int IMU_MESSAGE_PIPE          =5;
const int ACCELEROMETER_MESSAGE_PIPE =6;

const int INTEGER_TYPE = 0;
const int FLOAT_TYPE = 1;
const int BOOLEAN_TYPE = 2;

const int READ                      =0;
const int WRITE                     =1;