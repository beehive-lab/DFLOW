#include <canmodule.hpp>

CAN_example_message::CAN_example_message()
{
    enable_set = average_radius_set = temperature_set = false;
}

void CAN_example_message::setEnable(std::string message)
{ 
    if(message.find("Enabled")  != std::string::npos)
    {
        CAN_example_message::enable = true;
        CAN_example_message::enable_set = true; 
    }
    else if(message.find("Disabled") != std::string::npos)
    {
        CAN_example_message::enable = false;
        CAN_example_message::enable_set = true; 
    }
}

void CAN_example_message::setRadius(std::string message)
{ 
    if(message.find(":") != std::string::npos)
    {
        message = message.substr(message.find(":") + 2,4);
        CAN_example_message::average_radius = std::stof(message);
        CAN_example_message::average_radius_set = true; 
    }   
}

void CAN_example_message::setTemp(std::string message)
{ 
    if(message.find(":")  != std::string::npos)
    {
        message = message.substr(message.find(":") + 2,5);
        CAN_example_message::temperature = std::stof(message);
        CAN_example_message::temperature_set = true; 
    }   
}

const char* CAN_example_message::encodeForPipe()
{
    std::ostringstream os;
    os << enable <<","<< average_radius << "," << temperature;
    std::string s = os.str();
    return s.c_str();
}

void CAN_example_message::decodeFromPipe(const char* coded_message)
{
    std::string message = std::string(coded_message);
    if(message.size() > 1)
    {
        int enable_val = std::stoi(message.substr(0,2));
        if(enable_val)
            CAN_example_message::enable = true;
        else    
            CAN_example_message::enable = false;
        
        CAN_example_message::average_radius = std::stof(message.substr(2,3));
        CAN_example_message::temperature = std::stof(message.substr(6));
        enable_set = average_radius_set = temperature_set = true;
    }
    
}

bool CAN_example_message::messageReady()
{
    return enable_set&temperature_set&average_radius_set;
}

void CAN_example_message::reset()
{
    enable_set = false;
    average_radius_set = false;
    temperature_set = false;
}
