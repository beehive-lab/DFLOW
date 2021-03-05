#include <canmodule.hpp>
#include <boost/circular_buffer.hpp>
#include <pipes.hpp>
#include <vector>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <numeric>
#include <fcntl.h>
#include <future>
#include"engine_sensors_message.hpp"
#include"tpm_module_message.hpp"
#include"intake_sensors_message.hpp"
#include"imu_sensor_message.hpp"
#include"abs_module_message.hpp"
#include"configurable_modes_message.hpp"

#ifndef data_process_module_H
#define data_process_module_H

extern const int AVERAGE_OF_BUFFER;
extern const int MAX_OF_BUFFER;
extern const int MIN_OF_BUFFER;
extern const int SUM_OF_BUFFER;
extern const int FULL_BUFFER;
extern const int DO_NOT_COMPUTE;

class dataProcessing
{
    public:
        dataProcessing(std::vector<Pipes>,std::vector<Pipes>, std::vector<int>, int, int, int);
        void startProcessing(std::future<void>);
    private:
        void readCanPipes();
        void pushBackSignals();

        float compute_float_buffer_output(boost::circular_buffer<float>,int);
        bool compute_boolean_buffer_output(boost::circular_buffer<bool>,int);
        int compute_int_buffer_output(boost::circular_buffer<int>,int);

        void sendBooleanData(boost::circular_buffer<bool>,int);
        void sendIntegerData(boost::circular_buffer<int>,int);
        void sendFloatData(boost::circular_buffer<float>,int);

        time_t last_time;
        time_t last_tick_time;
        std::vector<Pipes> can_pipes_vector;
        std::vector<Pipes> output_pipe_vector;
        std::vector<int> data_mode_vector;
        int time_interval;
        int tick_interval;

        boost::circular_buffer<float>   air_temperature_buffer;
        boost::circular_buffer<int>     throttle_position_buffer;
        boost::circular_buffer<float>   tyre_pressure_front_buffer;
        boost::circular_buffer<float>   tyre_pressure_rear_buffer;
        boost::circular_buffer<int>     motorcycle_speed_buffer;
        boost::circular_buffer<int>     rear_wheel_speed_buffer;
        boost::circular_buffer<int>     front_wheel_speed_buffer;
        boost::circular_buffer<int>     brake_rear_active_buffer;
        boost::circular_buffer<int>     brake_front_active_buffer;
        boost::circular_buffer<int>     abs_mode_buffer;
        boost::circular_buffer<int>     tc_mode_buffer;
        boost::circular_buffer<int>     throttle_response_mode_buffer;
        boost::circular_buffer<float>   lean_angle_buffer;
        boost::circular_buffer<float>   battery_voltage_buffer;
        boost::circular_buffer<float>   oil_pressure_buffer;
        boost::circular_buffer<int>     gear_position_buffer;
        boost::circular_buffer<float>   water_temperature_buffer;
        boost::circular_buffer<int>     engine_speed_buffer;

        EngineSensorsMessage    received_engine_message;
        IntakeSensorsMessage    received_intake_message;
        TPMModuleMessage        received_tpm_message;
        ABSModuleMessage        received_abs_message;
        ConfigurableModesMessage received_config_message;
        IMUSensorMessage        received_imu_message;
};


#endif