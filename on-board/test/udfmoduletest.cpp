//Author: Radu-Tudor Andra
#include <string>
#include <ctime>
#include <vector>
#include <iostream>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <unistd.h>
#include "on_board_data_interface.hpp"
#include "pipes.hpp"
#include "edgeAI_functions.hpp"

namespace {

class UDFModuleTest : public ::testing::Test {

};


//test listener
TEST_F(UDFModuleTest, TestDataInterface) {
    std::vector<Pipes> dp_pipes_vector;
    Pipes config_pipe;

     //message which would come from cantools
    pipe(config_pipe.rdwr);
    for(int i = 0; i<SIGNAL_NUMBER; i++)
    {
        Pipes new_pipe;
        pipe(new_pipe.rdwr);
        dp_pipes_vector.push_back(new_pipe);
    }
    time_t time_test = time(0);
    float lean_angle = 55.5;
    int gear_position = 1;
    OnBoardDataInterface data_interface(dp_pipes_vector,config_pipe);
    write(dp_pipes_vector[TIMESTAMP].rdwr[WRITE], &time_test,sizeof(time_t));
    write(dp_pipes_vector[LEAN_ANGLE].rdwr[WRITE], &lean_angle, sizeof(float));
    write(dp_pipes_vector[GEAR_POSITION].rdwr[WRITE], &gear_position, sizeof(int));
    time_t received_time = data_interface.getSignalBatch();
    
    EXPECT_EQ(time_test, received_time);
    EXPECT_FLOAT_EQ(lean_angle,data_interface.getFloatData(LEAN_ANGLE));
    EXPECT_EQ(1,data_interface.getIntegerData(GEAR_POSITION));
    EXPECT_EQ(INT_MIN, data_interface.getIntegerData(MOTORCYCLE_SPEED));
}

TEST_F(UDFModuleTest, TestAddonFunctions) {
    std::vector<Pipes> dp_pipes_vector;
    Pipes config_pipe;

     //message which would come from cantools
    pipe(config_pipe.rdwr);
    for(int i = 0; i<SIGNAL_NUMBER; i++)
    {
        Pipes new_pipe;
        pipe(new_pipe.rdwr);
        dp_pipes_vector.push_back(new_pipe);
    }
    OnBoardDataInterface data_interface(dp_pipes_vector,config_pipe);
    DFLOW_Onboard_Addon_Functions AI_func;

    float lean_angle = 0,acceleration_x = 0, acceleration_y = 0;
    time_t time_test = time(0);

    write(dp_pipes_vector[TIMESTAMP].rdwr[WRITE], &time_test,sizeof(time_t));
    write(dp_pipes_vector[LEAN_ANGLE].rdwr[WRITE], &lean_angle, sizeof(float));
    write(dp_pipes_vector[ACCELERATION_X].rdwr[WRITE], &acceleration_x, sizeof(float));
    write(dp_pipes_vector[ACCELERATION_Y].rdwr[WRITE], &acceleration_y, sizeof(float));

    float result = AI_func.AIfunction(data_interface,std::vector<int>{LEAN_ANGLE,ACCELERATION_X,ACCELERATION_Y},
                                   std::vector<int>{FLOAT_UDF_DATA_TYPE,FLOAT_UDF_DATA_TYPE,FLOAT_UDF_DATA_TYPE},
                                   "./fdeep_crash_model.json");

    EXPECT_LE(result,0.5);
}

}