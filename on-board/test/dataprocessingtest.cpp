//Author: Radu-Tudor Andra
#include "data_process_module.hpp"
#include "pipes.hpp"
#include <string>
#include <iostream>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <unistd.h>
#include <thread>
#include <future>

unsigned int milisecond = 1000;

namespace {

std::vector<Pipes> can_pipe_test_vector;
std::vector<Pipes> output_pipes;

class DataProcessingTest : public ::testing::Test {
 protected:
  void SetUp() override {
    for(int i = 0; i<MESSAGE_NUMBER; i++)
    {
        Pipes new_pipe;
        pipe(new_pipe.rdwr);
        can_pipe_test_vector.push_back(new_pipe);
    }
    for(int i = 0; i<SIGNAL_NUMBER; i++)
    {
        Pipes new_pipe;
        pipe(new_pipe.rdwr);
        output_pipes.push_back(new_pipe);
    }
  }

  // void TearDown() override {}

};

void process_set_helper( std::vector<int> data_modes,std::shared_future<void> futureObj)
{

  dataProcessing processing_test_module = dataProcessing(can_pipe_test_vector,output_pipes,data_modes,1,3,3);
  processing_test_module.startProcessing(futureObj);
}

//test setter functions for example_message values
TEST_F(DataProcessingTest, Setters) {
  std::vector<int> data_modes{FULL_BUFFER,FULL_BUFFER,FULL_BUFFER,FULL_BUFFER,FULL_BUFFER,FULL_BUFFER,
                              FULL_BUFFER,FULL_BUFFER,DO_NOT_COMPUTE,DO_NOT_COMPUTE,DO_NOT_COMPUTE,FULL_BUFFER,
                              FULL_BUFFER,FULL_BUFFER,FULL_BUFFER,FULL_BUFFER,FULL_BUFFER,FULL_BUFFER,
                              FULL_BUFFER,FULL_BUFFER,FULL_BUFFER,FULL_BUFFER,FULL_BUFFER,FULL_BUFFER,
                              FULL_BUFFER,FULL_BUFFER};
  
  std::promise<void> exitSignal;
  std::shared_future<void> futureObj = exitSignal.get_future().share();
  
  std::thread test_data_process_thread(process_set_helper,data_modes,futureObj);
  IntakeSensorsMessage test_intake_message;
  test_intake_message.data.air_temperature = 10.1;
  test_intake_message.data.throttle_position = 5;
  write(can_pipe_test_vector[INTAKE_MESSAGE].rdwr[WRITE], &test_intake_message.data,sizeof(test_intake_message.data));
  EngineSensorsMessage test_engine_message;
  test_engine_message.data.battery_voltage = 12.7;
  test_engine_message.data.oil_pressure = 2.1;
  test_engine_message.data.gear_position = 0;
  test_engine_message.data.water_temperature = 72.3;
  test_engine_message.data.engine_speed = 2500;
  write(can_pipe_test_vector[ENGINE_MESSAGE].rdwr[WRITE], &test_engine_message.data,sizeof(test_engine_message.data));
  usleep(1000*milisecond);

  test_intake_message.data.air_temperature = 15.1;
  test_intake_message.data.throttle_position = 35;
  write(can_pipe_test_vector[INTAKE_MESSAGE].rdwr[WRITE], &test_intake_message.data,sizeof(test_intake_message.data));
  test_engine_message.data.battery_voltage = 13.8;
  test_engine_message.data.oil_pressure = 2.5;
  test_engine_message.data.gear_position = 0;
  test_engine_message.data.water_temperature = 80.5;
  test_engine_message.data.engine_speed = 7300;
  write(can_pipe_test_vector[ENGINE_MESSAGE].rdwr[WRITE], &test_engine_message.data,sizeof(test_engine_message.data));
  usleep(1000*milisecond);

  test_intake_message.data.air_temperature = 17.1;
  test_intake_message.data.throttle_position = 70;
  write(can_pipe_test_vector[INTAKE_MESSAGE].rdwr[WRITE], &test_intake_message.data,sizeof(test_intake_message.data));
  test_engine_message.data.battery_voltage = 13.8;
  test_engine_message.data.oil_pressure = 2.8;
  test_engine_message.data.gear_position = 0;
  test_engine_message.data.water_temperature = 91.5;
  test_engine_message.data.engine_speed = 11500;
  write(can_pipe_test_vector[ENGINE_MESSAGE].rdwr[WRITE], &test_engine_message.data,sizeof(test_engine_message.data));
  usleep(1000*milisecond);

  float air_temperature_buffer_linearized[3];
  int throttle_position_buffer_linearized[3];
  float battery_voltage_buffer_linearized[3];
  float oil_pressure_buffer_linearized[3];
  int gear_position_buffer_linearized[3];
  float water_temperature_buffer_linearized[3];
  int engine_speed_buffer_linearized[3];
  read(output_pipes[AIR_TEMPERATURE].rdwr[READ], air_temperature_buffer_linearized,  3*sizeof(float));
  read(output_pipes[THROTTLE_POSITION].rdwr[READ], throttle_position_buffer_linearized,  3*sizeof(int));
  read(output_pipes[BATTERY_VOLTAGE].rdwr[READ], battery_voltage_buffer_linearized,  3*sizeof(float));
  read(output_pipes[OIL_PRESSURE].rdwr[READ], oil_pressure_buffer_linearized,  3*sizeof(float));
  read(output_pipes[GEAR_POSITION].rdwr[READ], gear_position_buffer_linearized,  3*sizeof(int));
  read(output_pipes[WATER_TEMPERATURE].rdwr[READ], water_temperature_buffer_linearized,  3*sizeof(float));
  read(output_pipes[ENGINE_SPEED].rdwr[READ], engine_speed_buffer_linearized,  3*sizeof(int));

  ASSERT_THAT(air_temperature_buffer_linearized, testing::ElementsAre(10.1, 15.1, 17.1));
  ASSERT_THAT(throttle_position_buffer_linearized, testing::ElementsAre(5, 35, 70));
  ASSERT_THAT(battery_voltage_buffer_linearized, testing::ElementsAre(12.7, 13.8, 13.8));
  ASSERT_THAT(oil_pressure_buffer_linearized, testing::ElementsAre(2.1, 2.5, 2.8));
  ASSERT_THAT(gear_position_buffer_linearized, testing::ElementsAre(0, 0, 0));
  ASSERT_THAT(water_temperature_buffer_linearized, testing::ElementsAre(72.3, 80.5, 91.5));
  ASSERT_THAT(engine_speed_buffer_linearized, testing::ElementsAre(2500, 7300, 11500));

  exitSignal.set_value();
  test_data_process_thread.join();
}

}  // namespace