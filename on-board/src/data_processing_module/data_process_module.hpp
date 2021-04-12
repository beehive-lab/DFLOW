//Author: Radu-Tudor Andra
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
#include"profiling_message.hpp"

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

        std::vector<int> processing_inputs = {AIR_TEMPERATURE_PIPE, THROTTLE_POSITION_PIPE,
                                              TYRE_PRESSURE_FRONT_PIPE, TYRE_PRESSURE_REAR_PIPE, MOTORCYCLE_SPEED_PIPE,
                                              REAR_WHEEL_SPEED_PIPE, FRONT_WHEEL_SPEED_PIPE, BRAKE_REAR_ACTIVE_PIPE,
                                              BRAKE_FRONT_ACTIVE_PIPE, ABS_MODE_PIPE, TC_MODE_PIPE, 
                                              THROTTLE_RESPONSE_MODE_PIPE, LEAN_ANGLE_PIPE, BATTERY_VOLTAGE_PIPE,
                                              OIL_PRESSURE_PIPE, GEAR_POSITION_PIPE, WATER_TEMPERATURE_PIPE, 
                                              ENGINE_SPEED_PIPE, ACCELERATION_X_PIPE, ACCELERATION_Y_PIPE,
                                              ACCELERATION_Z_PIPE, CPU_USAGE_PIPE, CPU_FREQUENCY_PIPE,
                                              CPU_TEMPERATURE_PIPE, MEMORY_USAGE_PIPE};

        std::vector<int> processing_inputs_type = {FLOAT_TYPE, INTEGER_TYPE,
                                                   FLOAT_TYPE, FLOAT_TYPE, INTEGER_TYPE,
                                                   INTEGER_TYPE, INTEGER_TYPE, INTEGER_TYPE,
                                                   INTEGER_TYPE, INTEGER_TYPE, INTEGER_TYPE,
                                                   INTEGER_TYPE, FLOAT_TYPE, FLOAT_TYPE,
                                                   FLOAT_TYPE, INTEGER_TYPE, FLOAT_TYPE,
                                                   INTEGER_TYPE, FLOAT_TYPE, FLOAT_TYPE,
                                                   FLOAT_TYPE, FLOAT_TYPE, INTEGER_TYPE,
                                                   INTEGER_TYPE, INTEGER_TYPE};

        std::map<int,std::vector<float>>  float_buckets;
        std::map<int,std::vector<int>>    integer_buckets;
        std::map<int,std::vector<bool>> boolean_buckets;

        std::map<int,boost::circular_buffer<float>>  float_buffers;
        std::map<int,boost::circular_buffer<int>>    integer_buffers;
        std::map<int,boost::circular_buffer<bool>>   boolean_buffers;

        boost::circular_buffer<time_t>  time_stamp_buffer;

        EngineSensorsMessage    received_engine_message;
        IntakeSensorsMessage    received_intake_message;
        TPMModuleMessage        received_tpm_message;
        ABSModuleMessage        received_abs_message;
        ConfigurableModesMessage received_config_message;
        IMUSensorMessage        received_imu_message;
        AccelerometerMessage    received_accel_message;
        ProfilingMessage        received_profiling_message;

        bool new_engine_message;
        bool new_intake_message;
        bool new_tpm_message;
        bool new_abs_message;
        bool new_config_message;
        bool new_imu_message;
        bool new_accel_message;
        bool new_profiling_message;
};


#endif