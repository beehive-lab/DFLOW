//Author: Radu-Tudor Andra
#include "imu_sensor_message.hpp"

void IMUSensorMessage::set_from_map(std::map<std::string,std::string> value_map)
{
    IMUSensorMessage::data.lean_angle = std::stoi(value_map["LeanAngle"]);
}