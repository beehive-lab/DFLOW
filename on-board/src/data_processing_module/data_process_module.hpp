#include <canmodule.hpp>
#include <boost/circular_buffer.hpp>
#include <pipes.hpp>
#include <vector>
#include <stdio.h>
#include <time.h>
#include <numeric>

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
        dataProcessing(int *input_pipe, int, int, int);
        void setPipes(std::vector<Pipes>, std::vector<int>);
    private:
        float compute_float_buffer_output(boost::circular_buffer<float>,int);
        bool compute_boolean_buffer_output(boost::circular_buffer<bool>,int);
        int compute_int_buffer_output(boost::circular_buffer<int>,int);
        time_t last_time;
        time_t last_tick_time;
        int can_stream;
        int time_interval;
        int tick_interval;
        boost::circular_buffer<float> temperature_buffer;
        boost::circular_buffer<bool> switch_buffer;
        boost::circular_buffer<float> average_radius_buffer;
};


#endif