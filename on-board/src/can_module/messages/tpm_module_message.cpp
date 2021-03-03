#include "tpm_module_message.hpp"

void TPMModuleMessage::set_from_map(std::map<std::string,std::string> value_map)
{
    TPMModuleMessage::data.tyre_pressure_front = std::stod(value_map["TyrePressureFront"]);
    TPMModuleMessage::data.tyre_pressure_rear = std::stod(value_map["TyrePressureRear"]);
}