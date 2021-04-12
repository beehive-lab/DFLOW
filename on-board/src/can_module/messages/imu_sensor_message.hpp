//Author: Radu-Tudor Andra
#include <map>
#include <string>

#ifndef IMU_SENSOR_MESSAGE_H
#define IMU_SENSOR_MESSAGE_H

class IMUSensorMessage{

    public:
        struct{
            float lean_angle;
        }data;

        void set_from_map(std::map<std::string,std::string>);

};

#endif