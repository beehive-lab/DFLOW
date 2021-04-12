//Author: Radu-Tudor Andra
#include "intake_sensors_message.hpp"

void IntakeSensorsMessage::set_from_map(std::map<std::string,std::string> value_map)
{
    IntakeSensorsMessage::data.throttle_position = std::stoi(value_map["ThrottlePosition"]);
    IntakeSensorsMessage::data.air_temperature = std::stod(value_map["AirTemperature"]);
}