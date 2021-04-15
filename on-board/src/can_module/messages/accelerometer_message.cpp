//Author: Radu-Tudor Andra
#include "accelerometer_message.hpp"

void AccelerometerMessage::set_from_string(std::string str)
{
    std::vector<float> values;
    std::istringstream iss(str);
    std::string token;
    while (std::getline(iss, token, ','))
    {
        values.push_back(std::stof(token));
    }
    data.acceleration_x = values[0];
    data.acceleration_y = values[1];
    data.acceleration_z = values[2];
    empty_message = false;
}
