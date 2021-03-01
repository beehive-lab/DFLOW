#include "canmodule.hpp"
#include "data_process_module.hpp"
#include "pipes.hpp"
#include <string>
#include <iostream>
#include "gtest/gtest.h"
#include <unistd.h>
#include <thread>
#include <future>

unsigned int milisecond = 1000;

namespace {

std::vector<Pipes> pipe_test_vector;

class DataProcessingTest : public ::testing::Test {
 protected:
  void SetUp() override {
     for(int i = 0; i<4; i++)
    {
        Pipes new_pipe;
        pipe(new_pipe.rdwr);
        pipe_test_vector.push_back(new_pipe);
    }
  }

  // void TearDown() override {}

};

void process_set_helper( std::vector<int> data_modes,std::future<void> futureObj)
{

  dataProcessing processing_test_module = dataProcessing(pipe_test_vector[CAN_OUTPUT_PIPE].rdwr,1,3,3);
  processing_test_module.setPipes(pipe_test_vector,data_modes, std::move(futureObj));
}

//test setter functions for example_message values
TEST_F(DataProcessingTest, Setters) {
  std::vector<int> data_modes{FULL_BUFFER,FULL_BUFFER,FULL_BUFFER};
  
  std::promise<void> exitSignal;
  std::future<void> futureObj = exitSignal.get_future();
  
  std::thread test_data_process_thread(process_set_helper,data_modes,std::move(futureObj));
  write(pipe_test_vector[CAN_OUTPUT_PIPE].rdwr[WRITE], "0,3.2,260.5",20);
  usleep(1000*milisecond);
  write(pipe_test_vector[CAN_OUTPUT_PIPE].rdwr[WRITE], "1,3.4,262.5",20);
  usleep(1000*milisecond);
  write(pipe_test_vector[CAN_OUTPUT_PIPE].rdwr[WRITE], "0,3.6,264.5",20);
  usleep(1000*milisecond);
  //processing_test_module.setPipes(pipe_test_vector,data_modes);
  float temperature_buffer_linearized[3];
  float average_radius_buffer_linearized[3];
  bool switch_buffer_linearized[3];
  read(pipe_test_vector[TEMPERATURE_PIPE].rdwr[READ], temperature_buffer_linearized,  3*sizeof(float));
  read(pipe_test_vector[AVERAGE_RADIUS_PIPE].rdwr[READ], average_radius_buffer_linearized,  3*sizeof(float));
  read(pipe_test_vector[SWITCH_PIPE].rdwr[READ], switch_buffer_linearized,  3*sizeof(bool));
  EXPECT_FLOAT_EQ(temperature_buffer_linearized[0], 260.5);
  EXPECT_FLOAT_EQ(temperature_buffer_linearized[1], 262.5);
  EXPECT_FLOAT_EQ(temperature_buffer_linearized[2], 264.5);
  EXPECT_FLOAT_EQ(average_radius_buffer_linearized[0], 3.2);
  EXPECT_FLOAT_EQ(average_radius_buffer_linearized[1], 3.4);
  EXPECT_FLOAT_EQ(average_radius_buffer_linearized[2], 3.6);
  ASSERT_FALSE(switch_buffer_linearized[0]);
  ASSERT_TRUE(switch_buffer_linearized[1]);
  ASSERT_FALSE(switch_buffer_linearized[2]);
  exitSignal.set_value();
  test_data_process_thread.join();
}

}  // namespace