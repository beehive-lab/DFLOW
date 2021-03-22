#include <data_process_module.hpp>

//global variables definig data output modes
const int AVERAGE_OF_BUFFER=0;
const int MAX_OF_BUFFER=1;
const int MIN_OF_BUFFER=2;
const int SUM_OF_BUFFER=3;
const int FULL_BUFFER=4;
const int DO_NOT_COMPUTE=5;

//constructor
dataProcessing::dataProcessing(std::vector<Pipes> can_pipes_vector, std::vector<Pipes> output_pipe_vector, std::vector<int> data_mode_vector, int tick_interval, int interval, int buffer_size)
{
    //set up pipes and intervals
    dataProcessing::can_pipes_vector = can_pipes_vector;
    dataProcessing::output_pipe_vector = output_pipe_vector;
    dataProcessing::data_mode_vector = data_mode_vector;
    dataProcessing::tick_interval = tick_interval;
    dataProcessing::time_interval = interval;
    dataProcessing::last_time = time(0);

    //intialise buffers
    dataProcessing::air_temperature_buffer     =boost::circular_buffer<float>(buffer_size);
    dataProcessing::throttle_position_buffer   =boost::circular_buffer<int>(buffer_size);
    dataProcessing::tyre_pressure_front_buffer =boost::circular_buffer<float>(buffer_size);
    dataProcessing::tyre_pressure_rear_buffer  =boost::circular_buffer<float>(buffer_size);
    dataProcessing::motorcycle_speed_buffer    =boost::circular_buffer<int>(buffer_size);
    dataProcessing::rear_wheel_speed_buffer    =boost::circular_buffer<int>(buffer_size);
    dataProcessing::front_wheel_speed_buffer   =boost::circular_buffer<int>(buffer_size);
    dataProcessing::brake_rear_active_buffer   =boost::circular_buffer<int>(buffer_size);
    dataProcessing::brake_front_active_buffer  =boost::circular_buffer<int>(buffer_size);
    dataProcessing::abs_mode_buffer            =boost::circular_buffer<int>(buffer_size);
    dataProcessing::tc_mode_buffer             =boost::circular_buffer<int>(buffer_size);
    dataProcessing::throttle_response_mode_buffer=boost::circular_buffer<int>(buffer_size);
    dataProcessing::lean_angle_buffer          =boost::circular_buffer<float>(buffer_size);
    dataProcessing::battery_voltage_buffer     =boost::circular_buffer<float>(buffer_size);
    dataProcessing::oil_pressure_buffer        =boost::circular_buffer<float>(buffer_size);
    dataProcessing::gear_position_buffer       =boost::circular_buffer<int>(buffer_size);
    dataProcessing::water_temperature_buffer   =boost::circular_buffer<float>(buffer_size);
    dataProcessing::engine_speed_buffer        =boost::circular_buffer<int>(buffer_size);
    dataProcessing::time_stamp_buffer          =boost::circular_buffer<time_t>(buffer_size);

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
            return std::accumulate(buffer.begin(), buffer.end(),0.0)/buffer.size();
        }
        case SUM_OF_BUFFER:
        {
            return std::accumulate(buffer.begin(), buffer.end(),0.0);
        }
        case MAX_OF_BUFFER:
        {
            float *linearized_buffer = buffer.linearize();
            float max;
            for(int i = 0; i< buffer.size(); i++)
                if(linearized_buffer[i]>max)
                    max = linearized_buffer[i];
            return max;
        }
        case MIN_OF_BUFFER:
        {
            float *linearized_buffer = buffer.linearize();
            float min;
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
            return std::accumulate(buffer.begin(), buffer.end(),0)/buffer.size();
        }
        case SUM_OF_BUFFER:
        {
            return std::accumulate(buffer.begin(), buffer.end(),0);
        }
        case MAX_OF_BUFFER:
        {
            int *linearized_buffer = buffer.linearize();
            int max;
            for(int i = 0; i< buffer.size(); i++)
                if(linearized_buffer[i]>max)
                    max = linearized_buffer[i];
            return max;
        }
        case MIN_OF_BUFFER:
        {
            int *linearized_buffer = buffer.linearize();
            int min;
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

    //perform non-blocking reads
    ssize_t intake_r = read(can_pipes_vector[INTAKE_MESSAGE_PIPE].rdwr[READ],&temp_intake_message.data,sizeof(temp_intake_message.data));
    ssize_t tpm_r = read(can_pipes_vector[TPM_MESSAGE_PIPE].rdwr[READ],&temp_tpm_message.data,sizeof(temp_tpm_message.data));
    ssize_t abs_r = read(can_pipes_vector[ABS_MESSAGE_PIPE].rdwr[READ],&temp_abs_message.data,sizeof(temp_abs_message.data));
    ssize_t config_r = read(can_pipes_vector[CONFIG_MESSAGE_PIPE].rdwr[READ],&temp_config_message.data,sizeof(temp_config_message.data));
    ssize_t imu_r = read(can_pipes_vector[IMU_MESSAGE_PIPE].rdwr[READ],&temp_imu_message.data,sizeof(temp_imu_message.data));
    ssize_t engine_r = read(can_pipes_vector[ENGINE_MESSAGE_PIPE].rdwr[READ],&temp_engine_message.data,sizeof(temp_engine_message.data));

    //if read was succesful set current in-module messages
    if(intake_r>0)
    {
        received_intake_message.data = temp_intake_message.data;
    }
    if(tpm_r>0)
    {
        received_tpm_message.data = temp_tpm_message.data;
    }
    if(abs_r>0)
    {
        received_abs_message.data = temp_abs_message.data;
    }
    if(config_r>0)
    {
        received_config_message.data = temp_config_message.data;
    }
    if(imu_r>0)
    {
        received_imu_message.data = temp_imu_message.data;
    }
    if(engine_r>0)
    {
        received_engine_message.data = temp_engine_message.data;
    }
}

//helper function that pushes back the current module signals to the buffers
//takes the current time as parameter to set the timestamp buffer
void dataProcessing::pushBackSignals(time_t time)
{
    dataProcessing::air_temperature_buffer.push_back(received_intake_message.data.air_temperature);    
    dataProcessing::throttle_position_buffer.push_back(received_intake_message.data.throttle_position);

    dataProcessing::tyre_pressure_front_buffer.push_back(received_tpm_message.data.tyre_pressure_front);
    dataProcessing::tyre_pressure_rear_buffer.push_back(received_tpm_message.data.tyre_pressure_rear);

    dataProcessing::motorcycle_speed_buffer.push_back(received_abs_message.data.motorcycle_speed); 
    dataProcessing::rear_wheel_speed_buffer.push_back(received_abs_message.data.rear_wheel_speed);
    dataProcessing::front_wheel_speed_buffer.push_back(received_abs_message.data.front_wheel_speed);
    dataProcessing::brake_rear_active_buffer.push_back(received_abs_message.data.rear_brake_active); 
    dataProcessing::brake_front_active_buffer.push_back(received_abs_message.data.front_brake_active);

    dataProcessing::abs_mode_buffer.push_back(received_config_message.data.abs_mode);            
    dataProcessing::tc_mode_buffer.push_back(received_config_message.data.tc_mode);
    dataProcessing::throttle_response_mode_buffer.push_back(received_config_message.data.throttle_response_mode);

    dataProcessing::lean_angle_buffer.push_back(received_imu_message.data.lean_angle);

    dataProcessing::battery_voltage_buffer.push_back(received_engine_message.data.battery_voltage);     
    dataProcessing::oil_pressure_buffer.push_back(received_engine_message.data.oil_pressure);
    dataProcessing::gear_position_buffer.push_back(received_engine_message.data.gear_position);
    dataProcessing::water_temperature_buffer.push_back(received_engine_message.data.water_temperature);
    dataProcessing::engine_speed_buffer.push_back(received_engine_message.data.engine_speed);

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
    for(int i=0;i<6;i++)
        int retval = fcntl(can_pipes_vector[i].rdwr[READ],F_SETFL,fcntl(can_pipes_vector[i].rdwr[READ],F_GETFL) | O_NONBLOCK);

    
    while(futureObj.wait_for(std::chrono::milliseconds(1))== std::future_status::timeout)
    {
        dataProcessing::readCanPipes();

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

            //INTAKE_SENSORS
            dataProcessing::sendFloatData(air_temperature_buffer,AIR_TEMPERATURE_PIPE);
            dataProcessing::sendIntegerData(throttle_position_buffer,THROTTLE_POSITION_PIPE);

            //TPM_MODULE
            dataProcessing::sendFloatData(tyre_pressure_front_buffer,TYRE_PRESSURE_FRONT_PIPE);
            dataProcessing::sendFloatData(tyre_pressure_rear_buffer,TYRE_PRESSURE_REAR_PIPE);

            //ABS_MODULE
            dataProcessing::sendIntegerData(motorcycle_speed_buffer,MOTORCYCLE_SPEED_PIPE);
            dataProcessing::sendIntegerData(rear_wheel_speed_buffer,REAR_WHEEL_SPEED_PIPE);
            dataProcessing::sendIntegerData(front_wheel_speed_buffer,FRONT_WHEEL_SPEED_PIPE);
            dataProcessing::sendIntegerData(brake_rear_active_buffer,BRAKE_REAR_ACTIVE_PIPE);
            dataProcessing::sendIntegerData(brake_front_active_buffer,BRAKE_FRONT_ACTIVE_PIPE);

            //CONFIG_MODES
            dataProcessing::sendIntegerData(abs_mode_buffer,ABS_MODE_PIPE);
            dataProcessing::sendIntegerData(tc_mode_buffer,TC_MODE_PIPE);
            dataProcessing::sendIntegerData(throttle_position_buffer,THROTTLE_POSITION_PIPE);

            //IMU_MODULE
            dataProcessing::sendFloatData(lean_angle_buffer,LEAN_ANGLE_PIPE);
            
            //ENGINE_SENSORS
            dataProcessing::sendFloatData(battery_voltage_buffer,BATTERY_VOLTAGE_PIPE);
            dataProcessing::sendFloatData(oil_pressure_buffer,OIL_PRESSURE_PIPE);
            dataProcessing::sendIntegerData(gear_position_buffer,GEAR_POSITION_PIPE);
            dataProcessing::sendFloatData(water_temperature_buffer,WATER_TEMPERATURE_PIPE);
            dataProcessing::sendIntegerData(engine_speed_buffer,ENGINE_SPEED_PIPE);

            dataProcessing::sendTimeData(time_stamp_buffer,TIMESTAMP_PIPE);
        }
    }
}



