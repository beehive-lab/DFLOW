//Author: Radu-Tudor Andra
#include <fstream>
#include <string>
#include <unistd.h>
#include <sys/times.h>
#include <sys/resource.h>
#include "profiling_message.hpp"
#include "pipes.hpp"

#ifndef PROFILING_MODULE_H
#define PROFILING_MODULE_H

class ProfilingModule{
    public:
        ProfilingModule();
        void computeAndSendStats(Pipes);
        float getCPUUsage();
        int getMemoryUsage();
        int getCPUFrequency();
        int getCPUTemperature();
    private:
        clock_t lastCPU, lastSysCPU, lastUserCPU;
        int numProcessors;
        void init();
        int parseLine(std::string);
};

#endif