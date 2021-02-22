#include <vector>


#ifndef PIPES_H
#define PIPES_H

extern const int SWITCH_PIPE;
extern const int AVERAGE_RADIUS_PIPE;
extern const int TEMPERATURE_PIPE;
extern const int CAN_OUTPUT_PIPE;

extern const int READ;
extern const int WRITE;

enum pipe {SwitchPipe=0, AverageRadiusPipe=1, TemperaturePipe=2,CANOutputPipe=3};

struct Pipes
{
    int rdwr[2];
};

#endif