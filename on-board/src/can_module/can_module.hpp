//Author: Radu-Tudor Andra
#include<stdlib.h>
#include<string>
#include<map>
#include<future>
#include<fcntl.h>
#include"can_interface.hpp"
#include"pycan_interface.hpp"
#include"pipes.hpp"
#include"engine_sensors_message.hpp"
#include"tpm_module_message.hpp"
#include"intake_sensors_message.hpp"
#include"imu_sensor_message.hpp"
#include"abs_module_message.hpp"
#include"configurable_modes_message.hpp"
#include"file_interface.hpp"

#ifndef CAN_MODULE_H
#define CAN_MODULE_H

//this is the class providing the functionality for the CAN MODULE
class CAN_Module{
    public:
        CAN_Module(std::string,std::string,std::string,CAN_Interface* = new CAN_Python_Interface());
        CAN_Module(std::string,std::string,CAN_Interface* = new CAN_Python_Interface());
        void setListener(std::vector<Pipes>,Pipes, std::shared_future<void>);
        void sendConfigMessage(ConfigurableModesMessage);
    private:
        std::string dbc_file_path;
        std::string python_file_path;
        std::vector<Pipes> output_pipes;
        std::vector<Pipes> input_pipes;
        std::string accelerometer_file_path;
        CAN_Interface* interface_module;
};

#endif