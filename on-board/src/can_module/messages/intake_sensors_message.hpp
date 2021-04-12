//Author: Radu-Tudor Andra
#include <map>
#include <string>

#ifndef INTAKE_SENSORS_MESSAGE_H
#define INTAKE_SENSORS_MESSAGE_H

class IntakeSensorsMessage{

    public:
        struct{
            float air_temperature;

            int throttle_position;
        }data;

        void set_from_map(std::map<std::string,std::string>);

};

#endif