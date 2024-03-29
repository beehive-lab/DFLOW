//Author: Radu-Tudor Andra
#include <map>
#include <string>

#ifndef CONFIGURABLE_MODES_MESSAGE_H
#define CONFIGURABLE_MODES_MESSAGE_H

class ConfigurableModesMessage{

    public:
        struct{
            int abs_mode;
            int tc_mode;
            int throttle_response_mode;
        }data;
        void set_from_map(std::map<std::string,std::string>);
        std::map<std::string,int> get_message_map();

};

#endif