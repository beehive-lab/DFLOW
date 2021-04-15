//Author: Radu-Tudor Andra
#include "can_interface.hpp"
#include "can_module.hpp"
#include "intake_sensors_message.hpp"
#include "configurable_modes_message.hpp"
#include <string>
#include <map>
#include <pipes.hpp>
#include <vector>
#include <iostream>
#include <future>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <unistd.h>

namespace {

class MockCAN_Python_Interface : public CAN_Interface{
 public:
  MOCK_METHOD(void, initializeInterface, (std::string str1, std::string str2), (override));
  MOCK_METHOD((std::map<std::string,std::string>), getMessageMap, (), (override));
  MOCK_METHOD(void, sendMessage, ((std::map<std::string,int>) msg_map), (override));
};

class ModuleTest : public ::testing::Test {

};

void process_set_helper(std::vector<Pipes>can_pipes_vector, Pipes config_pipe, std::shared_future<void> futureObj)
{
    std::map<std::string,std::string> test_map;
    test_map.insert(std::pair<std::string,std::string>(std::string("MessageType"),std::string("IntakeSensors")));
    test_map.insert(std::pair<std::string,std::string>(std::string("AirTemperature"),std::string("10.1")));
    test_map.insert(std::pair<std::string,std::string>(std::string("ThrottlePosition"),std::string("35")));

    MockCAN_Python_Interface can_interface;
    EXPECT_CALL(can_interface, initializeInterface(std::string("./DFLOW"),std::string("test"))).Times(1);
    EXPECT_CALL(can_interface, getMessageMap()).WillRepeatedly(testing::Return(test_map));
    CAN_Module can_module = CAN_Module(std::string("./DFLOW"),std::string("test"),std::string("test"),&can_interface);

    can_module.setListener(can_pipes_vector, config_pipe, futureObj);
    
}

//test listener
TEST_F(ModuleTest, TestListener) {
    std::vector<Pipes> can_pipes_vector;
    Pipes config_pipe;

     //message which would come from cantools
    pipe(config_pipe.rdwr);
    for(int i = 0; i< MESSAGE_NUMBER; i++)
    {
        Pipes new_pipe;
        pipe(new_pipe.rdwr);
        can_pipes_vector.push_back(new_pipe);
    }
    std::promise<void> exitSignal;
    std::shared_future<void> futureObj = exitSignal.get_future().share();

    std::thread test_can_module_thread(process_set_helper,can_pipes_vector, config_pipe, futureObj);
    IntakeSensorsMessage test_received_message;
    read(can_pipes_vector[INTAKE_MESSAGE].rdwr[READ], &test_received_message.data, sizeof(test_received_message.data));

    EXPECT_FLOAT_EQ(test_received_message.data.air_temperature,10.1);
    EXPECT_EQ(test_received_message.data.throttle_position,35);
    exitSignal.set_value();
    test_can_module_thread.join();
}

TEST_F(ModuleTest,TestSender)
{
  MockCAN_Python_Interface can_interface;
  ConfigurableModesMessage test_message;
  test_message.data.abs_mode = 1;
  test_message.data.tc_mode = 2;
  test_message.data.throttle_response_mode = 3;
  EXPECT_CALL(can_interface,sendMessage(test_message.get_message_map())).Times(1);
  CAN_Module can_module = CAN_Module(std::string("./DFLOW"),std::string("test"),std::string("test"),&can_interface);

  //can_module.setInterface(&can_interface);
  can_module.sendConfigMessage(test_message);
}

}