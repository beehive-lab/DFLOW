#include <map>
#include <string>

#ifndef ENGINE_SENSORS_MESSAGE_H
#define ENGINE_SENSORS_MESSAGE_H

class EngineSensorsMessage{

    public:
        struct{
            float battery_voltage;

            float oil_pressure;

            int gear_position;

            float water_temperature;

            int engine_speed;
        }data;

        void set_from_map(std::map<std::string,std::string>);

};

#endif