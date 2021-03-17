#include "configurable_modes_message.hpp"

void ConfigurableModesMessage::set_from_map(std::map<std::string,std::string> value_map)
{
    ConfigurableModesMessage::data.abs_mode = std::stoi(value_map["ABSMode"]);
    ConfigurableModesMessage::data.tc_mode = std::stoi(value_map["TCMODE"]);
    ConfigurableModesMessage::data.throttle_response_mode = std::stoi(value_map["ThrottleResponseMode"]);
}

std::map<std::string,std::string> ConfigurableModesMessage::get_message_map()
{
    std::map<std::string,std::string> output_map;
    output_map.insert(std::pair<std::string, std::string>("ABSMode",std::to_string(data.abs_mode)));
    output_map.insert(std::pair<std::string, std::string>("TCMode",std::to_string(data.tc_mode)));
    output_map.insert(std::pair<std::string, std::string>("ThottleResponseMode",std::to_string(data.throttle_response_mode)));
    return output_map;
}