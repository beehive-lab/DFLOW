#include <string>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <boost/lexical_cast.hpp>

#ifndef CANMODULE_H
#define CANMODULE_H

//UNUSED, WILL BE REMOVED BEFORE RELEASE
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
class CAN_example_message{
    public:
        /**
        * True/False
        */
        bool switch_value;

        /**
        * Range: 0..50 (0..5 m)
        * Scale: 0.1
        */
        float average_radius;

        /**
        * Range:229.52..270.47 degK
        * Scale: 0.01
        */
        float temperature;

        CAN_example_message();

        void reset();
        const char* encodeForPipe();
        void decodeFromPipe(const char*);
        void addMessageLine(std::string);
        bool messageReady();

    private:
        enum ValueToken {Switch, AverageRadius, Temperature};

        std::string enum_to_string(ValueToken type);
        bool switch_value_set_flag,average_radius_set_flag,temperature_set_flag;
        void setSwitchValue(std::string);
        void setRadius(std::string);
        void setTemp(std::string);
};


#endif
