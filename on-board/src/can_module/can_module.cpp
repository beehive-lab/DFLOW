#include "can_module.hpp"

//constructor
CAN_Module::CAN_Module(std::string dbc_file_path, std::string python_file_path, CAN_Interface* set_interface)
{
    CAN_Module::dbc_file_path = dbc_file_path;
    CAN_Module::python_file_path = python_file_path;
    CAN_Module::interface_module = set_interface;
}


//will set the listener for the can_module, outputing to the given pipes
//the shared future parameter is designed to stop the listener so that the program can exit properly
void CAN_Module::setListener(std::vector<Pipes> output_pipes, std::shared_future<void> futureObj)
{
    //instantiate the python class that will handle the cantools library
    interface_module->initializeInterface(CAN_Module::dbc_file_path,CAN_Module::python_file_path);

    while(futureObj.wait_for(std::chrono::milliseconds(1))== std::future_status::timeout)
    {
        //get the message from the python class
        std::map<std::string, std::string> message_map = interface_module->getMessageMap();

        //check message type and write accordinglt
        if(message_map["MessageType"]== "IMUSensor")
        {

            IMUSensorMessage imu_message;
            imu_message.set_from_map(message_map);
            write(output_pipes[IMU_MESSAGE_PIPE].rdwr[WRITE], &imu_message.data, sizeof(imu_message.data));
        }
        else if(message_map["MessageType"]== "EngineSensors")
        {
            EngineSensorsMessage engine_message;
            engine_message.set_from_map(message_map);
            write(output_pipes[ENGINE_MESSAGE_PIPE].rdwr[WRITE], &engine_message.data, sizeof(engine_message.data));
        }
        else if(message_map["MessageType"]== "ABSModule")
        {
            ABSModuleMessage abs_message;
            abs_message.set_from_map(message_map);
            write(output_pipes[ABS_MESSAGE_PIPE].rdwr[WRITE], &abs_message.data, sizeof(abs_message.data));
        }
        else if(message_map["MessageType"]== "TPMModule")
        {
            TPMModuleMessage tpm_message;
            tpm_message.set_from_map(message_map);
            write(output_pipes[TPM_MESSAGE_PIPE].rdwr[WRITE], &tpm_message.data, sizeof(tpm_message.data));
        }
        else if(message_map["MessageType"]== "IntakeSensors")
        {
            IntakeSensorsMessage intake_message;
            intake_message.set_from_map(message_map);
            write(output_pipes[INTAKE_MESSAGE_PIPE].rdwr[WRITE], &intake_message.data, sizeof(intake_message.data));
        }
        else if(message_map["MessageType"]== "ConfigurableMode")
        {
            ConfigurableModesMessage config_message;
            config_message.set_from_map(message_map);
            write(output_pipes[CONFIG_MESSAGE_PIPE].rdwr[WRITE], &config_message.data, sizeof(config_message.data));
        }
    }

    //release python GLI
    Py_Finalize();
}

void CAN_Module::sendConfigMessage(ConfigurableModesMessage message)
{
    //instantiate python class
    interface_module->initializeInterface(CAN_Module::dbc_file_path, CAN_Module::python_file_path);

    //send config message
    interface_module->sendMessage(message.get_message_map());
}