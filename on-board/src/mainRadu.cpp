//Author:Radu-Tudor Andra
#include <unistd.h>
#include <string>
#include <thread>
#include "can_module.hpp"
#include "pipes.hpp"
#include "data_process_module.hpp"
#include "on_board_data_interface.hpp"
#include "profiling_module.hpp"
#include "config.hpp"
#include "edgeAI_functions.hpp"

using namespace std;

//second release

//vectors holding the multiple pipes
std::vector<Pipes> can_pipes_vector;
std::vector<Pipes> processed_pipes_vector;
Pipes config_pipe;

//set can module
void retrieve(std::shared_future<void> futureObj)
{
  CAN_Module can_module = CAN_Module(DFLOW_DBC_PATH,PYTHON_PATH,ACCELEROMETER_PATH);
  can_module.setListener(can_pipes_vector,config_pipe, futureObj);
}

//set data processing module
void set_data_processing_module(std::shared_future<void> futureObj)
{
  //each entry in the data modes vector determines what the output of the data processing pipe represents
  std::vector<int> data_modes{AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,
                              AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,
                              AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,
                              AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,
                              AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER};
  dataProcessing processing_module = dataProcessing(can_pipes_vector, processed_pipes_vector, data_modes, 1, 1, 1);
  processing_module.startProcessing(futureObj);
}

//set up listener to check data processing output
void check_data_from_dprocess()
{
  OnBoardDataInterface data_interface(processed_pipes_vector,config_pipe);
  DFLOW_Onboard_Addon_Functions AI_func;
  while(true)
  {
    time_t time_of_batch = data_interface.getSignalBatch();
    std::cout<<"Lean_Angle is "<<data_interface.getFloatData(LEAN_ANGLE)<<std::endl;
    std::cout<<"Air Temp is "<<data_interface.getFloatData(AIR_TEMPERATURE)<<std::endl;
    std::cout<<"Front Tyre Pressure is "<<data_interface.getFloatData(TYRE_PRESSURE_FRONT)<<std::endl;
    std::cout<<"Rear Tyre Press is "<<data_interface.getFloatData(TYRE_PRESSURE_REAR)<<std::endl;
    std::cout<<"Battery Voltage is "<<data_interface.getFloatData(BATTERY_VOLTAGE)<<std::endl;
    std::cout<<"Oil Pressure is "<<data_interface.getFloatData(OIL_PRESSURE)<<std::endl;
    std::cout<<"Water Temp is "<<data_interface.getFloatData(WATER_TEMPERATURE)<<std::endl;
    std::cout<<"----------------------"<<std::endl;
    std::cout<<"Throttle Pos is "<<data_interface.getIntegerData(THROTTLE_POSITION)<<std::endl;
    std::cout<<"Motorcycle Speed is "<<data_interface.getIntegerData(MOTORCYCLE_SPEED)<<std::endl;
    std::cout<<"Rear Wheel Speed is "<<data_interface.getIntegerData(REAR_WHEEL_SPEED)<<std::endl;
    std::cout<<"Front Wheel Speed is "<<data_interface.getIntegerData(FRONT_WHEEL_SPEED)<<std::endl;
    std::cout<<"Rear Brake is "<<data_interface.getIntegerData(BRAKE_REAR_ACTIVE)<<std::endl;
    std::cout<<"Front Brake is "<<data_interface.getIntegerData(BRAKE_FRONT_ACTIVE)<<std::endl;
    std::cout<<"ABS is "<<data_interface.getIntegerData(ABS_MODE)<<std::endl;
    std::cout<<"TC is "<<data_interface.getIntegerData(TC_MODE)<<std::endl;
    std::cout<<"Throttle Response is "<<data_interface.getIntegerData(THROTTLE_RESPONSE_MODE)<<std::endl;
    std::cout<<"Gear is "<<data_interface.getIntegerData(GEAR_POSITION)<<std::endl;
    std::cout<<"Engine Speed is "<<data_interface.getIntegerData(ENGINE_SPEED)<<std::endl;
    std::cout<<"----------------------"<<std::endl;
    std::cout<<"CPU Usage is "<<data_interface.getFloatData(CPU_USAGE)<<std::endl;
    std::cout<<"CPU Freq is "<<data_interface.getIntegerData(CPU_FREQUENCY)<<std::endl;
    std::cout<<"CPU Temp is "<<data_interface.getIntegerData(CPU_TEMPERATURE)<<std::endl;
    std::cout<<"Memory Usage is "<<data_interface.getIntegerData(MEMORY_USAGE)<<std::endl;
    std::cout<<"==================================================="<<std::endl;

    float result_of_function = AI_func.AIfunction(data_interface,std::vector<int>{LEAN_ANGLE,ACCELERATION_X,ACCELERATION_Y},
                                   std::vector<int>{FLOAT_UDF_DATA_TYPE,FLOAT_UDF_DATA_TYPE,FLOAT_UDF_DATA_TYPE},
                                   "./fdeep_crash_model.json");
    if(result_of_function > 0.6)
    {
      std::cout<<"XXXXXXXXXXMotorcycle crashedXXXXXXXXXXXXXX"<<std::endl;
    }
    else
    {
      std::cout<<"XXXXXXXXXXMotorcycle did not crashXXXXXXXXXXXXXX"<<std::endl;
    }
  }
}


void set_profiling_module(Pipes profiling_pipe)
{
  ProfilingModule profiling_module;
  while(true)
  {
    profiling_module.computeAndSendStats(profiling_pipe);
    usleep(1000000);
  }
}

int main() {

  //create exit signal to stop listeners on threads
  std::promise<void> exitSignal;
  std::future<void> futureObj = exitSignal.get_future();
  std::shared_future<void> shrdFutureObj = futureObj.share();

  pipe(config_pipe.rdwr);
  //initialize can pipes and data_proccesing pipes
  for(int i = 0; i<MESSAGE_NUMBER; i++)
  {
      Pipes new_pipe;
      pipe(new_pipe.rdwr);
      can_pipes_vector.push_back(new_pipe);
  }
  for(int i = 0; i<SIGNAL_NUMBER;i++)
  {
      Pipes new_pipe;
      pipe(new_pipe.rdwr);
      processed_pipes_vector.push_back(new_pipe);
  }

  //create threads
  std::thread can_thread(retrieve,shrdFutureObj);
  std::thread data_proccesing_thread(set_data_processing_module,shrdFutureObj);
  std::thread profiling_thread(set_profiling_module, can_pipes_vector[PROFILING_MESSAGE]);
  std::thread udf_thread(check_data_from_dprocess);
  can_thread.join();
  data_proccesing_thread.join();
  profiling_thread.join();
  udf_thread.join();
  return 0;
}
