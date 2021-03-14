#include <canmodule.hpp>

//UNUSED, WILL BE REMOVED BEFORE RELEASE
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
std::string CAN_example_message::enum_to_string(ValueToken type) {
   switch(type) {
      case Switch:
         return "Enable";
      case AverageRadius:
         return "AverageRadius";
      case Temperature:
         return "Temperature";
      default:
         return "";
   }
}

//check ilegal character when converting strings to floats
bool check_illegal_characters(const std::string &str)
{
    return str.find_first_not_of("0123456789. ") == std::string::npos;
}


CAN_example_message::CAN_example_message()
{
    switch_value_set_flag = average_radius_set_flag = temperature_set_flag = false;
}

//add a line from the candump and scan it, setting values accordingly
void CAN_example_message::addMessageLine(std::string cantools_line)
{
  if(cantools_line.find(enum_to_string(Switch)) != std::string::npos)
  {
    CAN_example_message::setSwitchValue(cantools_line);
  }
  if(cantools_line.find(enum_to_string(AverageRadius)) != std::string::npos)
  {
    CAN_example_message::setRadius(cantools_line);
  }
  if(cantools_line.find(enum_to_string(Temperature)) != std::string::npos)\
  {
    CAN_example_message::setTemp(cantools_line);
  }
}

//set values based on lines
void CAN_example_message::setSwitchValue(std::string message)
{ 
    if(message.find("Enabled")  != std::string::npos)
    {
        //set switch value
        CAN_example_message::switch_value = true;
        //indicate that the value has been filled and it is ready for encoding
        CAN_example_message::switch_value_set_flag = true; 
    }
    else if(message.find("Disabled") != std::string::npos)
    {
        //set switch value
        CAN_example_message::switch_value = false;
        //indicate that the value has been filled and it is ready for encoding
        CAN_example_message::switch_value_set_flag = true; 
    }
}

void CAN_example_message::setRadius(std::string message)
{ 
    if(message.find(":") != std::string::npos)
    {
        message = message.substr(message.find(":") + 2,4);
        if(check_illegal_characters(message))
        {
            CAN_example_message::average_radius = std::stof(message);
            CAN_example_message::average_radius_set_flag = true;
        } 
    }   
}

void CAN_example_message::setTemp(std::string message)
{ 
    if(message.find(":")  != std::string::npos)
    {
        message = message.substr(message.find(":") + 2,5);
        if(check_illegal_characters(message))
        {
            CAN_example_message::temperature = std::stof(message);
            CAN_example_message::temperature_set_flag = true; 
        }
    }   
}

//encode message for pipe transmision
const char* CAN_example_message::encodeForPipe()
{
    std::ostringstream os;
    os << switch_value <<","<< average_radius << "," << temperature;
    std::string s = os.str();
    return s.c_str();
}

//decode message sent through pipe
void CAN_example_message::decodeFromPipe(const char* coded_message)
{
    std::string message = std::string(coded_message);
    if(message.size() > 1)
    {
        unsigned first_comma = message.find_first_of(',');
        unsigned second_comma = message.find_last_of(',');

        std::string message_switch_val = message.substr(0,first_comma);
        std::string message_average_radius = message.substr(first_comma + 1, second_comma - first_comma - 1);
        std::string message_temperature = message.substr(second_comma + 1);

        int switch_value_val = std::stoi(message_switch_val);
        if(switch_value_val)
        {
            CAN_example_message::switch_value = true;
        }
        else    
        {
            CAN_example_message::switch_value = false;
        }
        

        CAN_example_message::average_radius = std::stof(message_average_radius);
        CAN_example_message::temperature = std::stof(message_temperature);
        switch_value_set_flag = average_radius_set_flag = temperature_set_flag = true;
    }
    
}

//check if all values of the message have been filled
bool CAN_example_message::messageReady()
{
    return switch_value_set_flag&temperature_set_flag&average_radius_set_flag;
}

//reset the values so the message structure can be reused
void CAN_example_message::reset()
{
    switch_value_set_flag = false;
    average_radius_set_flag = false;
    temperature_set_flag = false;
}
