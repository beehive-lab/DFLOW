//Author: Radu-Tudor Andra
#ifndef FILE_INTERFACE_H
#define FILE_INTERFACE_H

#include <cstring>
#include <string>
#include "accelerometer_message.hpp"
#include <fstream>

class File_Interface{
    public:
        File_Interface(std::string);
        AccelerometerMessage readAccelerometerFile();
    private:
        std::string accelerometer_file_path;
};

#endif