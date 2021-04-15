#include <unistd.h>
#include <string>
#include <thread>
#include "can_module.hpp"
#include "pipes.hpp"
#include "WifiComms.h"
#include "BluetoothComms.h"
#include "data_process_module.hpp"
#include "Logic.h"
#include "on_board_data_interface.hpp"
#include "profiling_module.hpp"
#include "config.hpp"

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
  dataProcessing processing_module = dataProcessing(can_pipes_vector, processed_pipes_vector, data_modes, 1, 2, 2);
  processing_module.startProcessing(futureObj);
}

//set up listener to check data processing output
void check_data_from_dprocess()
{
  OnBoardDataInterface data_interface(processed_pipes_vector,config_pipe);
  while(true)
  {
    time_t time_of_batch = data_interface.getSignalBatch();
    std::cout<<"Lean_Angle is "<<data_interface.getFloatData(LEAN_ANGLE)<<std::endl;
    std::cout<<"CPU Usage is "<<data_interface.getFloatData(CPU_USAGE)<<std::endl;
    std::cout<<"CPU Freq is "<<data_interface.getIntegerData(CPU_FREQUENCY)<<std::endl;
    std::cout<<"CPU Temp is "<<data_interface.getIntegerData(CPU_TEMPERATURE)<<std::endl;
    std::cout<<"Memory Usage is "<<data_interface.getIntegerData(MEMORY_USAGE)<<std::endl;
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

void logic_module_thread()
{
  OnBoardDataInterface data_interface(processed_pipes_vector, config_pipe);
  uint8_t key[10] = "password";
  Logic logic_module = Logic(&data_interface, false, false, key);
  logic_module.Wifi_logic(true, true, 8080);
}

int main() {

  //create exit signal to stop listeners on threads
  std::promise<void> exitSignal;
  std::future<void> futureObj = exitSignal.get_future();
  std::shared_future<void> shrdFutureObj = futureObj.share();

  pipe(config_pipe.rdwr);
  //initialize can pipes and data_proccesing pipes
  for(int i = 0; i < MESSAGE_NUMBER; i++)
  {
      Pipes new_pipe;
      pipe(new_pipe.rdwr);
      can_pipes_vector.push_back(new_pipe);
  }
  for(int i = 0; i < SIGNAL_NUMBER;i++)
  {
      Pipes new_pipe;
      pipe(new_pipe.rdwr);
      processed_pipes_vector.push_back(new_pipe);
  }

  //create threads
  std::thread can_thread(retrieve,shrdFutureObj);
  std::thread data_proccesing_thread(set_data_processing_module,shrdFutureObj);
  std::thread profiling_thread(set_profiling_module, can_pipes_vector[PROFILING_MESSAGE]);
//  std::thread udf_thread(check_data_from_dprocess);
  std::thread logic_thread(logic_module_thread);
  can_thread.join();
  data_proccesing_thread.join();
  profiling_thread.join();
//  udf_thread.join();
  logic_thread.join();
  return 0;
}
