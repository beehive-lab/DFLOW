#include "on_board_data_interface.hpp"
#include <iostream>
OnBoardDataInterface::OnBoardDataInterface(std::vector<Pipes> processed_pipes_vector)
{
    OnBoardDataInterface::processed_pipes_vector = processed_pipes_vector;
}

time_t OnBoardDataInterface::getSignalBatch(){
    //FOR NON_BLOCKING READS
    for(int i=0;i<22;i++)
        if(i != TIMESTAMP_PIPE)
            int retval = fcntl(processed_pipes_vector[i].rdwr[READ],F_SETFL,fcntl(processed_pipes_vector[i].rdwr[READ],F_GETFL) | O_NONBLOCK);
    

    if((read(processed_pipes_vector[TIMESTAMP_PIPE].rdwr[READ],&time_received,sizeof(time_t))))
    {   
        if(!(read(processed_pipes_vector[AIR_TEMPERATURE_PIPE].rdwr[READ],&air_temperature,sizeof(float)) > 0))
        air_temperature = -1;
        if(!(read(processed_pipes_vector[THROTTLE_POSITION_PIPE].rdwr[READ],&throttle_position,sizeof(int)) > 0))
        throttle_position = -1;
        if(!(read(processed_pipes_vector[TYRE_PRESSURE_FRONT_PIPE].rdwr[READ],&tyre_pressure_front,sizeof(float)) > 0))
        tyre_pressure_front = -1;
        if(!(read(processed_pipes_vector[TYRE_PRESSURE_REAR_PIPE].rdwr[READ],&tyre_pressure_rear,sizeof(float)) > 0))
        tyre_pressure_rear = -1;
        if(!(read(processed_pipes_vector[MOTORCYCLE_SPEED_PIPE].rdwr[READ],&motorcycle_speed,sizeof(int)) > 0))
        motorcycle_speed = -1;
        if(!(read(processed_pipes_vector[REAR_WHEEL_SPEED_PIPE].rdwr[READ],&rear_wheel_speed,sizeof(int)) > 0))
        rear_wheel_speed = -1;
        if(!(read(processed_pipes_vector[FRONT_WHEEL_SPEED_PIPE].rdwr[READ],&front_wheel_speed,sizeof(int)) > 0))
        front_wheel_speed = -1;
        if(!(read(processed_pipes_vector[BRAKE_REAR_ACTIVE_PIPE].rdwr[READ],&rear_brake_active,sizeof(int)) > 0))
        rear_brake_active = -1;
        if(!(read(processed_pipes_vector[BRAKE_FRONT_ACTIVE_PIPE].rdwr[READ],&front_brake_active,sizeof(int)) > 0))
        front_brake_active = -1;
        if(!(read(processed_pipes_vector[ABS_MODE_PIPE].rdwr[READ],&abs_mode,sizeof(int)) > 0))
        abs_mode = -1;
        if(!(read(processed_pipes_vector[TC_MODE_PIPE].rdwr[READ],&tc_mode,sizeof(int)) > 0))
        tc_mode = -1;
        if(!(read(processed_pipes_vector[THROTTLE_RESPONSE_MODE_PIPE].rdwr[READ],&throttle_response_mode,sizeof(int)) > 0))
        throttle_response_mode = -1;
        if(!(read(processed_pipes_vector[LEAN_ANGLE_PIPE].rdwr[READ],&lean_angle,sizeof(float)) > 0))
        lean_angle = -1;
        if(!(read(processed_pipes_vector[BATTERY_VOLTAGE_PIPE].rdwr[READ],&battery_voltage,sizeof(float)) > 0))
        battery_voltage = -1;
        if(!(read(processed_pipes_vector[OIL_PRESSURE_PIPE].rdwr[READ],&oil_pressure,sizeof(float)) > 0))
        oil_pressure = -1;
        if(!(read(processed_pipes_vector[GEAR_POSITION_PIPE].rdwr[READ],&gear_position,sizeof(int)) > 0))
        gear_position = -1;
        if(!(read(processed_pipes_vector[WATER_TEMPERATURE_PIPE].rdwr[READ],&water_temperature,sizeof(float)) > 0))
        water_temperature = -1;
        if(!(read(processed_pipes_vector[ENGINE_SPEED_PIPE].rdwr[READ],&engine_speed,sizeof(int)) > 0))
        engine_speed = -1;
        if(!(read(processed_pipes_vector[ACCELERATION_X_PIPE].rdwr[READ],&acceleration_x,sizeof(float)) > 0))
        acceleration_x = -1;
        if(!(read(processed_pipes_vector[ACCELERATION_Y_PIPE].rdwr[READ],&acceleration_y,sizeof(float)) > 0))
        acceleration_y = -1;
        if(!(read(processed_pipes_vector[ACCELERATION_Z_PIPE].rdwr[READ],&acceleration_z,sizeof(float)) > 0))
        acceleration_z = -1;
    }
    //RETURN TO BLOCKING
    for(int i=0;i<22;i++)
            int retval = fcntl(processed_pipes_vector[i].rdwr[READ],F_SETFL,fcntl(processed_pipes_vector[i].rdwr[READ],F_GETFL) | ~O_NONBLOCK);
    return time_received;
}

float OnBoardDataInterface::getFloatData(int data_id)
{
    if(data_id == AIR_TEMPERATURE_PIPE)
        return air_temperature;
    else if(data_id == TYRE_PRESSURE_FRONT_PIPE)
        return tyre_pressure_front;
    else if(data_id == TYRE_PRESSURE_REAR_PIPE)
        return tyre_pressure_rear;
    else if(data_id == LEAN_ANGLE_PIPE)
        return lean_angle;
    else if(data_id == BATTERY_VOLTAGE_PIPE)
        return battery_voltage;
    else if(data_id == OIL_PRESSURE_PIPE)
        return oil_pressure;
    else if(data_id == WATER_TEMPERATURE_PIPE)
        return water_temperature;
    else if(data_id == ACCELERATION_X_PIPE)
        return acceleration_x;
    else if(data_id == ACCELERATION_Y_PIPE)
        return acceleration_y;
    else if(data_id == ACCELERATION_Z_PIPE)
        return acceleration_z;
    else
        return -1;
}

int OnBoardDataInterface::getIntegerData(int data_id)
{
    if(data_id = THROTTLE_POSITION_PIPE)
        return throttle_position;
    else if(data_id == MOTORCYCLE_SPEED_PIPE)
        return motorcycle_speed;
    else if(data_id == REAR_WHEEL_SPEED_PIPE)
        return rear_wheel_speed;
    else if(data_id == FRONT_WHEEL_SPEED_PIPE)
        return front_wheel_speed;
    else if(data_id == BRAKE_REAR_ACTIVE_PIPE)
        return rear_brake_active;
    else if(data_id == BRAKE_FRONT_ACTIVE_PIPE)
        return front_brake_active;
    else if(data_id == ABS_MODE_PIPE)
        return abs_mode;
    else if(data_id == TC_MODE_PIPE)
        return tc_mode;
    else if(data_id == THROTTLE_RESPONSE_MODE_PIPE)
        return throttle_response_mode;
    else if(data_id == GEAR_POSITION_PIPE)
        return gear_position;
    else if(data_id == ENGINE_SPEED_PIPE)
        return engine_speed;
    else
        return -1;
}

bool OnBoardDataInterface::getBooleanData(int data_id)
{
    return NULL;
}
