//Author: Radu-Tudor Andra
#include <data_process_module.hpp>

//global variables definig data output modes
const int AVERAGE_OF_BUFFER=0;
const int MAX_OF_BUFFER=1;
const int MIN_OF_BUFFER=2;
const int SUM_OF_BUFFER=3;
const int FULL_BUFFER=4;
const int DO_NOT_COMPUTE=5;

//constructor
dataProcessing::dataProcessing(std::vector<Pipes> can_pipes_vector, std::vector<Pipes> output_pipe_vector,
                               std::vector<int> data_mode_vector, int tick_interval, int interval, int buffer_size)
{
    //set up pipes and intervals
    dataProcessing::can_pipes_vector = can_pipes_vector;
    dataProcessing::output_pipe_vector = output_pipe_vector;
    dataProcessing::data_mode_vector = data_mode_vector;
    dataProcessing::tick_interval = tick_interval;
    dataProcessing::time_interval = interval;
    dataProcessing::last_time = time(0);

    //creater buffers and buckets
    for(int i = 0; i < processing_inputs.size(); i++)
    {
        if(processing_inputs_type[i] == INTEGER_TYPE)
        {
            integer_buffers.insert(std::pair<int,boost::circular_buffer<int>>(processing_inputs[i],boost::circular_buffer<int>(buffer_size)));
            integer_buckets.insert(std::pair<int,std::vector<int>>(processing_inputs[i],std::vector<int>()));
        }
        else if(processing_inputs_type[i] == FLOAT_TYPE)
        {
            float_buffers.insert(std::pair<int,boost::circular_buffer<float>>(processing_inputs[i],boost::circular_buffer<float>(buffer_size)));
            float_buckets.insert(std::pair<int,std::vector<float>>(processing_inputs[i],std::vector<float>()));
        }
        else if(processing_inputs_type[i] == BOOLEAN_TYPE)
        {
            boolean_buffers.insert(std::pair<int,boost::circular_buffer<bool>>(processing_inputs[i],boost::circular_buffer<bool>(buffer_size)));
            boolean_buckets.insert(std::pair<int,std::vector<bool>>(processing_inputs[i],std::vector<bool>()));
        }   
    }

    dataProcessing::time_stamp_buffer = boost::circular_buffer<time_t>(buffer_size);
}

//compute ouput of different type buffers based on data mode(taken from data_index - which is the global variable defining the respective pipe)

//for time
time_t dataProcessing::compute_time_buffer_output(boost::circular_buffer<time_t> buffer,int data_index)
{
    switch (dataProcessing::data_mode_vector[data_index]){
        case AVERAGE_OF_BUFFER:
        {
            return std::accumulate(buffer.begin(), buffer.end(),0.0)/buffer.size();
        }
        case SUM_OF_BUFFER:
        {
            return std::accumulate(buffer.begin(), buffer.end(),0.0);
        }
        case MAX_OF_BUFFER:
        {
            time_t *linearized_buffer = buffer.linearize();
            time_t max;
            for(int i = 0; i< buffer.size(); i++)
                if(linearized_buffer[i]>max)
                    max = linearized_buffer[i];
            return max;
        }
        case MIN_OF_BUFFER:
        {
            time_t *linearized_buffer = buffer.linearize();
            time_t min;
            for(int i = 0; i< buffer.size(); i++)
                if(linearized_buffer[i]<min)
                    min = linearized_buffer[i];
            return min;
        }
        default:
            return 0;
    }
        
}

//for floats
float dataProcessing::compute_float_buffer_output(boost::circular_buffer<float> buffer,int data_index)
{
    switch (dataProcessing::data_mode_vector[data_index]){
        case AVERAGE_OF_BUFFER:
        {
            if(buffer.size()>0)
                return std::accumulate(buffer.begin(), buffer.end(),0.0)/buffer.size();
            else
                return 0;
        }
        case SUM_OF_BUFFER:
        {
            return std::accumulate(buffer.begin(), buffer.end(),0.0);
        }
        case MAX_OF_BUFFER:
        {
            float *linearized_buffer = buffer.linearize();
            float max = FLT_MIN;
            for(int i = 0; i< buffer.size(); i++)
                if(linearized_buffer[i]>max)
                    max = linearized_buffer[i];
            return max;
        }
        case MIN_OF_BUFFER:
        {
            float *linearized_buffer = buffer.linearize();
            float min = FLT_MAX;
            for(int i = 0; i< buffer.size(); i++)
                if(linearized_buffer[i]<min)
                    min = linearized_buffer[i];
            return min;
        }
        default:
            return 0;
    }
        
}

//for booleans
bool dataProcessing::compute_boolean_buffer_output(boost::circular_buffer<bool> buffer,int data_index)
{
    switch (dataProcessing::data_mode_vector[data_index]){
        case AVERAGE_OF_BUFFER:
        {
            float sum = std::accumulate(buffer.begin(), buffer.end(),0.0)/buffer.size();
            if(sum > 0.5)
                return true;
            else
                return false;
            break;
        }
        case MAX_OF_BUFFER:
        {
            bool *linearized_buffer = buffer.linearize();
            for(int i = 0; i< buffer.size(); i++)
                if(linearized_buffer[i])
                    return true;
            return false;
        }
        case MIN_OF_BUFFER:
        {
            bool *linearized_buffer = buffer.linearize();
            for(int i = 0; i< buffer.size(); i++)
                if(!linearized_buffer[i])
                    return false;
            return true;
        }
        default:
            return false;
    }
        
}

//for ints
int dataProcessing::compute_int_buffer_output(boost::circular_buffer<int> buffer,int data_index)
{
    switch (dataProcessing::data_mode_vector[data_index]){
        case AVERAGE_OF_BUFFER:
        {
            if(buffer.size() > 0)
                return std::accumulate(buffer.begin(), buffer.end(),0)/buffer.size();
            else
                return 0;
        }
        case SUM_OF_BUFFER:
        {
            return std::accumulate(buffer.begin(), buffer.end(),0);
        }
        case MAX_OF_BUFFER:
        {
            int *linearized_buffer = buffer.linearize();
            int max = INT_MIN;
            for(int i = 0; i< buffer.size(); i++)
                if(linearized_buffer[i]>max)
                    max = linearized_buffer[i];
            return max;
        }
        case MIN_OF_BUFFER:
        {
            int *linearized_buffer = buffer.linearize();
            int min = INT_MAX;
            for(int i = 0; i< buffer.size(); i++)
                if(linearized_buffer[i]<min)
                    min = linearized_buffer[i];
            return min;
        }
        default:
            return 0;
    }
        
}

//helper function that reads the can pipes for incoming messages
void dataProcessing::readCanPipes()
{
    //temporary messages to hold data
    EngineSensorsMessage    temp_engine_message;
    IntakeSensorsMessage    temp_intake_message;
    TPMModuleMessage        temp_tpm_message;
    ABSModuleMessage        temp_abs_message;
    ConfigurableModesMessage temp_config_message;
    IMUSensorMessage        temp_imu_message;
    AccelerometerMessage    temp_accel_message;
    ProfilingMessage        temp_profiling_message;

    //perform non-blocking reads
    ssize_t intake_r = read(can_pipes_vector[INTAKE_MESSAGE].rdwr[READ],&temp_intake_message.data,sizeof(temp_intake_message.data));
    ssize_t tpm_r = read(can_pipes_vector[TPM_MESSAGE].rdwr[READ],&temp_tpm_message.data,sizeof(temp_tpm_message.data));
    ssize_t abs_r = read(can_pipes_vector[ABS_MESSAGE].rdwr[READ],&temp_abs_message.data,sizeof(temp_abs_message.data));
    ssize_t config_r = read(can_pipes_vector[CONFIG_MESSAGE].rdwr[READ],&temp_config_message.data,sizeof(temp_config_message.data));
    ssize_t imu_r = read(can_pipes_vector[IMU_MESSAGE].rdwr[READ],&temp_imu_message.data,sizeof(temp_imu_message.data));
    ssize_t engine_r = read(can_pipes_vector[ENGINE_MESSAGE].rdwr[READ],&temp_engine_message.data,sizeof(temp_engine_message.data));
    ssize_t accel_r = read(can_pipes_vector[ACCELEROMETER_MESSAGE].rdwr[READ],&temp_accel_message.data,sizeof(temp_accel_message.data));
    ssize_t profiling_r = read(can_pipes_vector[PROFILING_MESSAGE].rdwr[READ],&temp_profiling_message.data,sizeof(temp_profiling_message.data));

    //if read was succesful set current in-module messages
    if(intake_r>0)
    {
        received_intake_message.data = temp_intake_message.data;
        new_intake_message = true;
    }
    if(tpm_r>0)
    {
        received_tpm_message.data = temp_tpm_message.data;
        new_tpm_message = true;
    }
    if(abs_r>0)
    {
        received_abs_message.data = temp_abs_message.data;
        new_abs_message = true;
    }
    if(config_r>0)
    {
        received_config_message.data = temp_config_message.data;
        new_config_message = true;
    }
    if(imu_r>0)
    {
        received_imu_message.data = temp_imu_message.data;
        new_imu_message = true;
    }
    if(engine_r>0)
    {
        received_engine_message.data = temp_engine_message.data;
        new_engine_message = true;
    }
    if(accel_r>0)
    {
        received_accel_message = temp_accel_message;
        new_accel_message = true;
    }
    if(profiling_r>0)
    {
        received_profiling_message = temp_profiling_message;
        new_profiling_message = true;
    }
}

//!!this function performs the mapping between messages and signal buckets/buffers!!
//Push any new messages to buckets
void dataProcessing::pushBackToBuckets()
{
    if(new_intake_message)
    {
        float_buckets[AIR_TEMPERATURE].push_back(received_intake_message.data.air_temperature);
        integer_buckets[THROTTLE_POSITION].push_back(received_intake_message.data.throttle_position);
        new_intake_message = false;
    }

    if(new_tpm_message)
    {
        float_buckets[TYRE_PRESSURE_FRONT].push_back(received_tpm_message.data.tyre_pressure_front);
        float_buckets[TYRE_PRESSURE_REAR].push_back(received_tpm_message.data.tyre_pressure_rear);
        new_tpm_message = false;
    }

    if(new_abs_message)
    {
        integer_buckets[MOTORCYCLE_SPEED].push_back(received_abs_message.data.motorcycle_speed); 
        integer_buckets[REAR_WHEEL_SPEED].push_back(received_abs_message.data.rear_wheel_speed);
        integer_buckets[FRONT_WHEEL_SPEED].push_back(received_abs_message.data.front_wheel_speed);
        integer_buckets[BRAKE_REAR_ACTIVE].push_back(received_abs_message.data.rear_brake_active); 
        integer_buckets[BRAKE_FRONT_ACTIVE].push_back(received_abs_message.data.front_brake_active);
        new_abs_message = false;
    }

    if(new_config_message)
    {
        integer_buckets[ABS_MODE].push_back(received_config_message.data.abs_mode);            
        integer_buckets[TC_MODE].push_back(received_config_message.data.tc_mode);
        integer_buckets[THROTTLE_RESPONSE_MODE].push_back(received_config_message.data.throttle_response_mode);
        new_config_message = false;
    }

    if(new_imu_message)
    {
        float_buckets[LEAN_ANGLE].push_back(received_imu_message.data.lean_angle);
        new_imu_message = false;
    }

    if(new_engine_message)
    {
        float_buckets[BATTERY_VOLTAGE].push_back(received_engine_message.data.battery_voltage);     
        float_buckets[OIL_PRESSURE].push_back(received_engine_message.data.oil_pressure);
        integer_buckets[GEAR_POSITION].push_back(received_engine_message.data.gear_position);
        float_buckets[WATER_TEMPERATURE].push_back(received_engine_message.data.water_temperature);
        integer_buckets[ENGINE_SPEED].push_back(received_engine_message.data.engine_speed);
        new_engine_message = false;
    }

    if(new_accel_message)
    {
        float_buckets[ACCELERATION_X].push_back(received_accel_message.data.acceleration_x);
        float_buckets[ACCELERATION_Y].push_back(received_accel_message.data.acceleration_y);
        float_buckets[ACCELERATION_Z].push_back(received_accel_message.data.acceleration_z);
        new_accel_message = false;
    }

    if(new_profiling_message)
    {
        float_buckets[CPU_USAGE].push_back(received_profiling_message.data.cpu_usage);
        integer_buckets[CPU_FREQUENCY].push_back(received_profiling_message.data.cpu_freq);
        integer_buckets[CPU_TEMPERATURE].push_back(received_profiling_message.data.cpu_temp);
        integer_buckets[MEMORY_USAGE].push_back(received_profiling_message.data.memory_usage);
    }
}

//clear buckets after pushing to buffers
void dataProcessing::clearBuckets()
{
    for(int i = 0; i <  processing_inputs.size(); i++)
    {
        if(processing_inputs_type[i] == INTEGER_TYPE)
            integer_buckets[processing_inputs[i]].clear();
        else if(processing_inputs_type[i] == FLOAT_TYPE)
            float_buckets[processing_inputs[i]].clear();
        else if(processing_inputs_type[i] == BOOLEAN_TYPE)
            boolean_buckets[processing_inputs[i]].clear();
    }
}

int getIntegerAverageOfVector(std::vector<int> v)
{
    if(v.size() > 0)
        return std::accumulate(v.begin(),v.end(), 0) / static_cast<int>(v.size());
    else
        return INT_MIN;
}

float getFloatAverageOfVector(std::vector<float> v)
{
    if(v.size() > 0)
        return static_cast<float>(std::accumulate(v.begin(),v.end(), 0.0))/ static_cast<float>(v.size());
    else
        return FLT_MIN;
}

bool getBooleanAverageOfVector(std::vector<bool> v)
{
    if(v.size() > 0)
        if(static_cast<float>(std::accumulate(v.begin(),v.end(), 0.0))/ static_cast<float>(v.size()) >= 0.5)
            return true;
        else
            return false;
    else
        return 0;
}

//helper function that pushes back the current module signals to the buffers
//takes the current time as parameter to set the timestamp buffer
void dataProcessing::pushBackSignals(time_t time)
{

    for(int i = 0; i <  processing_inputs.size(); i++)
    {
        if(processing_inputs_type[i] == INTEGER_TYPE)
        {
            int bucket_data = getIntegerAverageOfVector(integer_buckets[processing_inputs[i]]);
            if(bucket_data != INT_MIN)
                integer_buffers[processing_inputs[i]].push_back(bucket_data);
        }
        else if(processing_inputs_type[i] == FLOAT_TYPE)
        {
            float bucket_data = getFloatAverageOfVector(float_buckets[processing_inputs[i]]);
            if(bucket_data != FLT_MIN)
                float_buffers[processing_inputs[i]].push_back(getFloatAverageOfVector(float_buckets[processing_inputs[i]]));
        }
        else if(processing_inputs_type[i] == BOOLEAN_TYPE)
        {
            boolean_buffers[processing_inputs[i]].push_back(getBooleanAverageOfVector(boolean_buckets[processing_inputs[i]]));
        }
    }

    dataProcessing::clearBuckets();

    dataProcessing::time_stamp_buffer.push_back(time);
}


//helper functions for sending data based on type, either as full buffers or as computed output

//for ints
void dataProcessing::sendIntegerData(boost::circular_buffer<int> buffer, int data_index)
{
    if(data_mode_vector[data_index] == FULL_BUFFER)
    {
        int* linearized_buffer = buffer.linearize();
        write(output_pipe_vector[data_index].rdwr[WRITE], linearized_buffer, buffer.size()*sizeof(int));   
    }
    else if(data_mode_vector[data_index] != DO_NOT_COMPUTE)
    {
        int output = compute_int_buffer_output(buffer, data_index);
        write(output_pipe_vector[data_index].rdwr[WRITE], &output, sizeof(int));
    }
}

//for floats
void dataProcessing::sendFloatData(boost::circular_buffer<float> buffer, int data_index)
{
    if(data_mode_vector[data_index] == FULL_BUFFER)
    {
        float* linearized_buffer = buffer.linearize();
        write(output_pipe_vector[data_index].rdwr[WRITE], linearized_buffer, buffer.size()*sizeof(float));   
    }
    else if(data_mode_vector[data_index] != DO_NOT_COMPUTE)
    {
        float output = compute_float_buffer_output(buffer, data_index);
        write(output_pipe_vector[data_index].rdwr[WRITE], &output, sizeof(float));
    }
}

//for bools
void dataProcessing::sendBooleanData(boost::circular_buffer<bool> buffer, int data_index)
{
    if(data_mode_vector[data_index] == FULL_BUFFER)
    {
        bool* linearized_buffer = buffer.linearize();
        write(output_pipe_vector[data_index].rdwr[WRITE], linearized_buffer, buffer.size()*sizeof(bool));   
    }
    else if(data_mode_vector[data_index] != DO_NOT_COMPUTE)
    {
        bool output = compute_boolean_buffer_output(buffer, data_index);
        write(output_pipe_vector[data_index].rdwr[WRITE], &output, sizeof(bool));
    }
}

//for time
void dataProcessing::sendTimeData(boost::circular_buffer<time_t> buffer, int data_index)
{
    if(data_mode_vector[data_index] == FULL_BUFFER)
    {
        time_t* linearized_buffer = buffer.linearize();
        write(output_pipe_vector[data_index].rdwr[WRITE], linearized_buffer, buffer.size()*sizeof(time_t));   
    }
    else if(data_mode_vector[data_index] != DO_NOT_COMPUTE)
    {
        time_t output = compute_time_buffer_output(buffer, data_index);
        write(output_pipe_vector[data_index].rdwr[WRITE], &output, sizeof(time_t));
    }
}

//start the module, must be given a shared future object to receive the stop signal 
void dataProcessing::startProcessing(std::shared_future<void> futureObj)
{

    //set times to current time
    dataProcessing::last_time = time(0);
    dataProcessing::last_tick_time = time(0);

    //make can output pipes non-blocking
    for(int i=0;i<8;i++)
        int retval = fcntl(can_pipes_vector[i].rdwr[READ],F_SETFL,fcntl(can_pipes_vector[i].rdwr[READ],F_GETFL) | O_NONBLOCK);

    
    while(futureObj.wait_for(std::chrono::milliseconds(1))== std::future_status::timeout)
    {
        dataProcessing::readCanPipes();
        dataProcessing::pushBackToBuckets();

        //if the tick interval has passed push back signals to buffer
        if(difftime(time(0),last_tick_time) >= tick_interval)
        {
            dataProcessing::pushBackSignals(time(0));
            last_tick_time = time(0);
        }

        //if the time interval has passed send data to pipes
        if(difftime(time(0),last_time) >= time_interval)
        {
            last_time = time(0);

            for(int i = 0; i <  processing_inputs.size(); i++)
            {
                if(processing_inputs_type[i] == INTEGER_TYPE)
                    sendIntegerData(integer_buffers[processing_inputs[i]], processing_inputs[i]);
                else if(processing_inputs_type[i] == FLOAT_TYPE)
                    sendFloatData(float_buffers[processing_inputs[i]], processing_inputs[i]);
                else if(processing_inputs_type[i] == BOOLEAN_TYPE)
                    sendBooleanData(boolean_buffers[processing_inputs[i]], processing_inputs[i]);
            }

            dataProcessing::sendTimeData(time_stamp_buffer,TIMESTAMP);
        }
    }
}



