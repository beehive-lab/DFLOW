#include "canmodule.hpp"
#include "dataretriever.hpp"
#include <string>
#include <iostream>
#include "gtest/gtest.h"
#include <unistd.h>

namespace {

class CANExampleMessageTest : public ::testing::Test {
 protected:
  void SetUp() override {
    //message which would come from cantools
     cantools_line[0] = std::string("  vcan0  000001F0   [8]  40 01 40 00 00 00 00 00 ::");
     cantools_line[1] = std::string("ExampleMessage(");
     cantools_line[2] = std::string("Enable: 'Disabled' -,");
     cantools_line[3] = std::string("AverageRadius: 3.2 m,");
     cantools_line[4] = std::string("Temperature: 250.1 degK");
     cantools_line[5] = std::string(")");
  }

  // void TearDown() override {}

  std::string cantools_line[6];
};

//test setter functions for example_message values
TEST_F(CANExampleMessageTest, Setters) {
  CAN_example_message example_message = CAN_example_message();

  for(int i = 0; i <= 5; i++)
  {
    example_message.addMessageLine(cantools_line[i]);
  }


  EXPECT_EQ(false,example_message.switch_value);
  EXPECT_FLOAT_EQ(3.2,example_message.average_radius);
  EXPECT_FLOAT_EQ(250.1, example_message.temperature);

  ASSERT_TRUE(example_message.messageReady());

  example_message.reset();
  ASSERT_FALSE(example_message.messageReady());
}

//check encode and decode functions of example_messages
TEST_F(CANExampleMessageTest, PipeEncodeDecode) {
  CAN_example_message example_encode_message = CAN_example_message();
  CAN_example_message example_decoded_message = CAN_example_message();

  example_encode_message.switch_value = true;
  example_encode_message.average_radius = 3.2;
  example_encode_message.temperature = 250.1;

  std::string encoded_message = std::string(example_encode_message.encodeForPipe());

  ASSERT_TRUE(encoded_message.compare(std::string("1,3.2,250.1")) == 0);


  example_decoded_message.decodeFromPipe(encoded_message.c_str());

  EXPECT_TRUE(example_decoded_message.switch_value);
  EXPECT_FLOAT_EQ(3.2,example_decoded_message.average_radius);
  EXPECT_FLOAT_EQ(250.1, example_decoded_message.temperature);
}

//check the dataRetriever functionality
class DataRetrieverTest : public ::testing::Test {
 protected:
  void SetUp() override {
      data_retriever.testing = true;
  }

  // void TearDown() override {}
  CAN_example_message received_message;
  dataRetriever data_retriever = {};
};

TEST_F(DataRetrieverTest, Assign) {
  int pip[2];

  data_retriever.listen(pip);

  received_message = data_retriever.example_message;

  ASSERT_FALSE(received_message.switch_value);
  ASSERT_FLOAT_EQ(received_message.average_radius,3.2);
  ASSERT_FLOAT_EQ(received_message.temperature,250.1);

}

}  // namespace