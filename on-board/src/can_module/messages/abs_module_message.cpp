#include "abs_module_message.hpp"

void ABSModuleMessage::set_from_map(std::map<std::string,std::string> value_map)
{
    ABSModuleMessage::data.motorcycle_speed = std::stoi(value_map["MotorcycleSpeed"]);
    ABSModuleMessage::data.front_wheel_speed = std::stoi(value_map["FrontWheelSpeed"]);
    ABSModuleMessage::data.rear_wheel_speed = std::stoi(value_map["RearWheelSpeed"]);
    ABSModuleMessage::data.rear_brake_active = std::stoi(value_map["BrakeRearActive"]);
    ABSModuleMessage::data.front_brake_active = std::stoi(value_map["BrakeFrontActive"]);
}