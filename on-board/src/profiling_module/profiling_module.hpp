#include <iostream>
#include <fstream>
#include <string>
#include <sys/times.h>
#include <sys/resource.h>

#ifndef PROFILING_MODULE_H
#define PROFILING_MODULE_H

class ProfilingModule{
    public:
        ProfilingModule();
        double getCPUUsage();
        int getMemoryUsage();
    private:
        clock_t lastCPU, lastSysCPU, lastUserCPU;
        int numProcessors;
        void init();
        int parseLine(std::string);
};

#endif