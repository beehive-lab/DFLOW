#include <boost/circular_buffer.hpp>
#include <pipes.hpp>
#include <vector>
#include <stdio.h>
#include <unistd.h>
#include <ctime>
#include <numeric>
#include <fcntl.h>
#include <future>
#include"engine_sensors_message.hpp"
#include"tpm_module_message.hpp"
#include"intake_sensors_message.hpp"
#include"imu_sensor_message.hpp"
#include"abs_module_message.hpp"
#include"configurable_modes_message.hpp"
#include"accelerometer_message.hpp"

#ifndef data_process_module_H
#define data_process_module_H

extern const int AVERAGE_OF_BUFFER;
extern const int MAX_OF_BUFFER;
extern const int MIN_OF_BUFFER;
extern const int SUM_OF_BUFFER;
extern const int FULL_BUFFER;
extern const int DO_NOT_COMPUTE;

//this is the dataProcessing class that comprises the functionality of the data processing module
class dataProcessing
{
    public:
        dataProcessing(std::vector<Pipes>,std::vector<Pipes>, std::vector<int>, int, int, int);
        void startProcessing(std::shared_future<void>);
    private:
        void readCanPipes();
        void pushBackSignals(time_t);
        void pushBackToBuckets();
        void clearBuckets();

        float compute_float_buffer_output(boost::circular_buffer<float>,int);
        bool compute_boolean_buffer_output(boost::circular_buffer<bool>,int);
        int compute_int_buffer_output(boost::circular_buffer<int>,int);
        time_t compute_time_buffer_output(boost::circular_buffer<time_t>,int);

        void sendBooleanData(boost::circular_buffer<bool>,int);
        void sendIntegerData(boost::circular_buffer<int>,int);
        void sendFloatData(boost::circular_buffer<float>,int);
        void sendTimeData(boost::circular_buffer<time_t>,int);

        time_t last_time;
        time_t last_tick_time;
        std::vector<Pipes> can_pipes_vector;
        std::vector<Pipes> output_pipe_vector;
        std::vector<int> data_mode_vector;
        int time_interval;
        int tick_interval;

        std::vector<float>   air_temperature_bucket;
        std::vector<int>     throttle_position_bucket;
        std::vector<float>   tyre_pressure_front_bucket;
        std::vector<float>   tyre_pressure_rear_bucket;
        std::vector<int>     motorcycle_speed_bucket;
        std::vector<int>     rear_wheel_speed_bucket;
        std::vector<int>     front_wheel_speed_bucket;
        std::vector<int>     brake_rear_active_bucket;
        std::vector<int>     brake_front_active_bucket;
        std::vector<int>     abs_mode_bucket;
        std::vector<int>     tc_mode_bucket;
        std::vector<int>     throttle_response_mode_bucket;
        std::vector<float>   lean_angle_bucket;
        std::vector<float>   battery_voltage_bucket;
        std::vector<float>   oil_pressure_bucket;
        std::vector<int>     gear_position_bucket;
        std::vector<float>   water_temperature_bucket;
        std::vector<int>     engine_speed_bucket;
        std::vector<float>   acceleration_x_bucket;
        std::vector<float>   acceleration_y_bucket;
        std::vector<float>   acceleration_z_bucket;

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
        boost::circular_buffer<float>     acceleration_x_buffer;
        boost::circular_buffer<float>     acceleration_y_buffer;
        boost::circular_buffer<float>     acceleration_z_buffer;
        boost::circular_buffer<time_t>  time_stamp_buffer;

        EngineSensorsMessage    received_engine_message;
        IntakeSensorsMessage    received_intake_message;
        TPMModuleMessage        received_tpm_message;
        ABSModuleMessage        received_abs_message;
        ConfigurableModesMessage received_config_message;
        IMUSensorMessage        received_imu_message;
        AccelerometerMessage    received_accel_message;

        bool new_engine_message;
        bool new_intake_message;
        bool new_tpm_message;
        bool new_abs_message;
        bool new_config_message;
        bool new_imu_message;
        bool new_accel_message;
};


#endif