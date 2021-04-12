#include <string>
#include <cstring>
#include <vector>
#include <sstream>


#ifndef PROFILING_MESSAGE_H
#define PROFILING_MESSAGE_H

class ProfilingMessage{

    public:
        struct{
            float cpu_usage;
            int   memory_usage;
            int   cpu_temp;
            int   cpu_freq;
        }data;
};

#endif