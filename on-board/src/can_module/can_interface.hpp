#include<Python.h>
#include<stdlib.h>
#include<iostream>
#include<string>
#include<map>
#include"pipes.hpp"
#include"engine_sensors_message.hpp"
#include"tpm_module_message.hpp"
#include"intake_sensors_message.hpp"
#include"imu_sensor_message.hpp"
#include"abs_module_message.hpp"
#include"configurable_modes_message.hpp"


#ifndef CAN_INTERFACE_H
#define CAN_INTERFACE_H

class CAN_Interface{
    public:
        CAN_Interface(std::string,std::string);
        void setListener(std::vector<Pipes>);
        void sendConfigMessage(ConfigurableModesMessage);
    private:
        std::string dbc_file_path;
        std::string python_file_path;
        PyObject* instantiatePythonClass();
};

#endif