//Author: Radu-Tudor Andra
#include "file_interface.hpp"
#include <iostream>
File_Interface::File_Interface(std::string accelerometer_file_path)
{
    File_Interface::accelerometer_file_path = accelerometer_file_path;
}

AccelerometerMessage File_Interface::readAccelerometerFile()
{
    std::string line;
    std::ifstream file_stream(accelerometer_file_path.c_str());
    //read first line of the file and set the message from it
    if(std::getline(file_stream,line))
    {
        AccelerometerMessage new_message;
        new_message.set_from_string(line);
        return new_message;
    }
    else  //return an empty message if there was nothing in the file
    {
        AccelerometerMessage empty_message;
        return empty_message;
    }
}