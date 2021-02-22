#include <data_process_module.hpp>

const int AVERAGE_OF_BUFFER=0;
const int MAX_OF_BUFFER=1;
const int MIN_OF_BUFFER=2;
const int SUM_OF_BUFFER=3;
const int FULL_BUFFER=4;
const int DO_NOT_COMPUTE=5;


dataProcessing::dataProcessing(int *input_pipe, int tick_interval, int interval, int buffer_size)
{
    dataProcessing::temperature_buffer = boost::circular_buffer<float>(buffer_size);
    dataProcessing::average_radius_buffer = boost::circular_buffer<float>(buffer_size);
    dataProcessing::switch_buffer = boost::circular_buffer<bool>(buffer_size);
    dataProcessing::can_stream = input_pipe[0];
    dataProcessing::time_interval = interval;
    dataProcessing::last_time = time(0);
}

float dataProcessing::compute_float_buffer_output(boost::circular_buffer<float> buffer,int data_mode)
{
    switch (data_mode){
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

bool dataProcessing::compute_boolean_buffer_output(boost::circular_buffer<bool> buffer,int data_mode)
{
    switch (data_mode){
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

int dataProcessing::compute_int_buffer_output(boost::circular_buffer<int> buffer,int data_mode)
{
    switch (data_mode){
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

void dataProcessing::setPipes(std::vector<Pipes> pipe_vector, std::vector<int> data_mode_vector)
{
    dataProcessing::last_time = time(0);
    dataProcessing::last_tick_time = time(0);
    CAN_example_message received_message = CAN_example_message();
    char can_output_string[200];
    while(true)
    {
        read(can_stream,can_output_string,200);
        received_message.decodeFromPipe(can_output_string);
        if(difftime(time(0),last_tick_time) >= tick_interval)
        {
            dataProcessing::temperature_buffer.push_back(received_message.temperature);
            dataProcessing::average_radius_buffer.push_back(received_message.average_radius);
            dataProcessing::switch_buffer.push_back(received_message.switch_value);
        }
        if(difftime(time(0),last_time) >= time_interval)
        {
            last_time = time(0);

            if(data_mode_vector[TEMPERATURE_PIPE] == FULL_BUFFER)
            {
                float* linearized_temperature_buffer = temperature_buffer.linearize();
                write(pipe_vector[TEMPERATURE_PIPE].rdwr[WRITE], linearized_temperature_buffer, temperature_buffer.size()*sizeof(float));
            }
            else if(data_mode_vector[TEMPERATURE_PIPE] != DO_NOT_COMPUTE)
            {
                float output = compute_float_buffer_output(temperature_buffer,data_mode_vector[TEMPERATURE_PIPE]);
                write(pipe_vector[TEMPERATURE_PIPE].rdwr[WRITE], &output, sizeof(float));
            }

            if(data_mode_vector[AVERAGE_RADIUS_PIPE] == FULL_BUFFER)
            {
                float* linearized_average_radius_buffer = average_radius_buffer.linearize();
                write(pipe_vector[AVERAGE_RADIUS_PIPE].rdwr[WRITE], linearized_average_radius_buffer, average_radius_buffer.size()*sizeof(float));
            }
            else if(data_mode_vector[AVERAGE_RADIUS_PIPE] != DO_NOT_COMPUTE)
            {
                float output = compute_float_buffer_output(average_radius_buffer,data_mode_vector[AVERAGE_RADIUS_PIPE]);
                write(pipe_vector[AVERAGE_RADIUS_PIPE].rdwr[WRITE], &output, sizeof(float));
            }

            if(data_mode_vector[SWITCH_PIPE] == FULL_BUFFER)
            {
                bool* linearized_switch_buffer = switch_buffer.linearize();
                write(pipe_vector[SWITCH_PIPE].rdwr[WRITE], linearized_switch_buffer, switch_buffer.size()*sizeof(bool));   
            }
            else if(data_mode_vector[SWITCH_PIPE] != DO_NOT_COMPUTE)
            {
                bool output = compute_boolean_buffer_output(switch_buffer, data_mode_vector[SWITCH_PIPE]);
                write(pipe_vector[SWITCH_PIPE].rdwr[WRITE], &output, sizeof(bool));
            }
        }
    }
}




uint8_t *message_pointer = NULL;

