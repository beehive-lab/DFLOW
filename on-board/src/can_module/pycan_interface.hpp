#include <string>
#include <map>
#include <Python.h>
#include <iostream>
#include <can_interface.hpp>

#ifndef CAN_Python_Interface_H
#define CAN_Python_Interface_H

//this is the CAN interface implementation that provides the means of interfacing with the can bus through the python cantools library
class CAN_Python_Interface : public CAN_Interface{
    public:
        void initializeInterface(std::string, std::string) override;
        std::map<std::string, std::string> getMessageMap() override;
        void sendMessage(std::map<std::string,std::string>) override;
        bool checkClassInstantiated();
    private:
        PyObject* python_can_class = nullptr;
};

#endif