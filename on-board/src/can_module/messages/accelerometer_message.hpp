#include <string>
#include <cstring>
#include <vector>
#include <sstream>


#ifndef ACCELEROMETER_MESSAGE_H
#define ACCELEROMETER_MESSAGE_H

class AccelerometerMessage{

    public:
        struct{
            float acceleration_x;
            float acceleration_y;
            float acceleration_z;
        }data;

        bool empty_message = true;

        void set_from_string(std::string);

};

#endif