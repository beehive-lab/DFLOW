#include "can_interface.hpp"
#include "pipes.hpp"
#include "configurable_modes_message.hpp"

//test project for Python embedding
int main()
{
    std::vector<Pipes> test_vector;
    CAN_Interface can_interface = CAN_Interface(std::string("./DFLOW.dbc"), std::string("wa"));
    ConfigurableModesMessage message;
    message.data.abs_mode = 1;
    message.data.tc_mode = 2;
    message.data.throttle_response_mode = 3;
    can_interface.sendConfigMessage(message);
}