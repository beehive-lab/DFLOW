//Author: Radu-Tudor Andra
#include "profiling_module.hpp"

ProfilingModule::ProfilingModule()
{
    ProfilingModule::init();
}

void ProfilingModule::computeAndSendStats(Pipes profiling_pipe)
{
    ProfilingMessage profiling_message;
    profiling_message.data.cpu_freq = getCPUFrequency();
    profiling_message.data.memory_usage = getMemoryUsage();
    profiling_message.data.cpu_temp = getCPUTemperature();
    profiling_message.data.cpu_usage = getCPUUsage();
    write(profiling_pipe.rdwr[WRITE],&profiling_message.data, sizeof(profiling_message.data));
}


void ProfilingModule::init()
{
    std::ifstream data_file;
    struct tms timeSample;
    std::string line;

    lastCPU = times(&timeSample);
    lastSysCPU = lastCPU;
    lastUserCPU = lastCPU;

    data_file.open("/proc/cpuinfo");
    numProcessors = 0;
    while(std::getline(data_file, line)){
        if (line.compare(0,9,"processor") == 0) numProcessors++;
    }
    data_file.close();
}

float ProfilingModule::getCPUUsage()
{
    struct tms timeSample;
    clock_t now;
    float percent;

    now = times(&timeSample);
    int who = RUSAGE_SELF;
    struct rusage usage;
    int ret;


    ret = getrusage(who, &usage);
    int user_usage_since_last_time = usage.ru_utime.tv_usec - lastUserCPU;
    int sys_usage_since_last_time = usage.ru_stime.tv_usec - lastSysCPU;
    if (now <= lastCPU || usage.ru_stime.tv_usec < lastSysCPU ||
        usage.ru_utime.tv_usec < lastUserCPU)
    {
        //Overflow detection. Just skip this value.
        percent = -1.0;
    }
    else
    {
        percent = user_usage_since_last_time + sys_usage_since_last_time;
        percent /= (now - lastCPU)*10000;
        percent /= numProcessors;
        percent *= 100;
    }
    lastCPU = now;
    lastSysCPU = usage.ru_stime.tv_usec;
    lastUserCPU = usage.ru_utime.tv_usec;

    return percent;
}


int ProfilingModule::parseLine(std::string line){
    // This assumes that a digit will be found and the line ends in " Kb".
    int index = 0;
    while (line[index] <'0' || line[index] > '9') index++;
    line = line.substr(index,line.size() - 3);
    int i = stoi(line);
    return i;
}

int ProfilingModule::getMemoryUsage()
{
    std::ifstream memory_file;
    memory_file.open("/proc/self/status");
    int result = -1;
    std::string line;

    while (std::getline(memory_file, line)){
        if (line.compare(0,6,"VmRSS:") == 0){
            result = parseLine(line);
            break;
        }
    }
    memory_file.close();
    return result;
}

int ProfilingModule::getCPUTemperature()
{
    std::ifstream temperature_file;
    int temp;
    temperature_file.open("/sys/class/thermal/thermal_zone0/temp");
    temperature_file>>temp;
    return temp/1000;
}

int ProfilingModule::getCPUFrequency()
{
    std::ifstream frequency_file;
    int freq;
    frequency_file.open("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq");
    frequency_file>>freq;
    return freq/1000;
}
