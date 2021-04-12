//Author: Radu-Tudor Andra
#include "engine_sensors_message.hpp"

void EngineSensorsMessage::set_from_map(std::map<std::string,std::string> value_map)
{
    EngineSensorsMessage::data.battery_voltage = std::stod(value_map["BatteryVoltage"]);
    EngineSensorsMessage::data.oil_pressure = std::stod(value_map["OilPressure"]);
    EngineSensorsMessage::data.gear_position = std::stoi(value_map["GearPosition"]);
    EngineSensorsMessage::data.water_temperature = std::stod(value_map["WaterTemperature"]);
    EngineSensorsMessage::data.engine_speed = std::stoi(value_map["EngineSpeed"]);
}