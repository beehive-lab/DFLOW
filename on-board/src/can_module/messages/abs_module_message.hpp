//Author: Radu-Tudor Andra
#include <map>
#include <string>
#include <iostream>

#ifndef ABS_MODULE_MESSAGE_H
#define ABS_MODULE_MESSAGE_H

class ABSModuleMessage{

    public:
        struct{
            int motorcycle_speed;

            int rear_wheel_speed;

            int front_wheel_speed;

            int rear_brake_active;

            int front_brake_active;
        }data;

        void set_from_map(std::map<std::string,std::string>);

};

#endif