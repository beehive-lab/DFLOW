//Author: Radu-Tudor Andra
#include "configurable_modes_message.hpp"

void ConfigurableModesMessage::set_from_map(std::map<std::string,std::string> value_map)
{
    ConfigurableModesMessage::data.abs_mode = std::stoi(value_map["ABSMode"]);
    ConfigurableModesMessage::data.tc_mode = std::stoi(value_map["TCMode"]);
    ConfigurableModesMessage::data.throttle_response_mode = std::stoi(value_map["ThrottleResponseMode"]);
}

std::map<std::string,int> ConfigurableModesMessage::get_message_map()
{
    std::map<std::string,int> output_map;
    output_map.insert(std::pair<std::string, int>("ABSMode",data.abs_mode));
    output_map.insert(std::pair<std::string, int>("TCMode",data.tc_mode));
    output_map.insert(std::pair<std::string, int>("ThottleResponseMode",data.throttle_response_mode));
    return output_map;
}