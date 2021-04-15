//Author: Radu-Tudor Andra
#include <pipes.hpp>

const int SIGNAL_NUMBER = 26;
const int MESSAGE_NUMBER = 8;

const int AIR_TEMPERATURE      =0;
const int THROTTLE_POSITION    =1;
const int TYRE_PRESSURE_FRONT  =2;
const int TYRE_PRESSURE_REAR   =3;
const int MOTORCYCLE_SPEED     =4;
const int REAR_WHEEL_SPEED     =5;
const int FRONT_WHEEL_SPEED    =6;
const int BRAKE_REAR_ACTIVE    =7;
const int BRAKE_FRONT_ACTIVE   =8;
const int ABS_MODE             =9;
const int TC_MODE              =10;
const int THROTTLE_RESPONSE_MODE=11;
const int LEAN_ANGLE           =12;
const int BATTERY_VOLTAGE      =13;
const int OIL_PRESSURE         =14;
const int GEAR_POSITION        =15;
const int WATER_TEMPERATURE    =16;
const int ENGINE_SPEED         =17;
const int ACCELERATION_X       =18;
const int ACCELERATION_Y       =19;
const int ACCELERATION_Z       =20;
const int TIMESTAMP            =21;
const int CPU_USAGE            =22;
const int CPU_FREQUENCY        =23;
const int CPU_TEMPERATURE      =24;
const int MEMORY_USAGE         =25;

const int INTAKE_MESSAGE       =0;
const int ABS_MESSAGE          =1;
const int TPM_MESSAGE          =2;
const int ENGINE_MESSAGE       =3;
const int CONFIG_MESSAGE       =4;
const int IMU_MESSAGE          =5;
const int ACCELEROMETER_MESSAGE =6;
const int PROFILING_MESSAGE    =7;

const int INTEGER_TYPE = 0;
const int FLOAT_TYPE = 1;
const int BOOLEAN_TYPE = 2;

const int READ                      =0;
const int WRITE                     =1;