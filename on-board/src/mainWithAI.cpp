#include <unistd.h>
#include <string>
#include <thread>
#include "can_module.hpp"
#include "pipes.hpp"
#include "data_process_module.hpp"
#include "on_board_data_interface.hpp"
#include "edgeAI_functions.hpp"
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
  can_module.setListener(can_pipes_vector, config_pipe, futureObj);
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
  dataProcessing processing_module = dataProcessing(can_pipes_vector, processed_pipes_vector, data_modes, 1, 3, 3);
  processing_module.startProcessing(futureObj);
}

//set up listener to check data processing output
void check_data_from_dprocess()
{
  OnBoardDataInterface data_interface(processed_pipes_vector, config_pipe);
  DFLOW_Onboard_Addon_Functions AI_func;
  while(true)
  {
    float result_of_function = AI_func.AIfunction(data_interface,std::vector<int>{LEAN_ANGLE_PIPE,ACCELERATION_X_PIPE,ACCELERATION_Y_PIPE},
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
    std::cout<<"CPU Usage is "<<profiling_module.getCPUUsage()<<std::endl;
    std::cout<<"Memory Usage is "<<profiling_module.getMemoryUsage()<<std::endl;
    usleep(3000000);
  }
}

int main() {

  //create exit signal to stop listeners on threads
  std::promise<void> exitSignal;
  std::future<void> futureObj = exitSignal.get_future();
  std::shared_future<void> shrdFutureObj = futureObj.share();
  
  //initialize can pipes and data_proccesing pipes
  for(int i = 0; i<8; i++)

  {
      Pipes new_pipe;
      pipe(new_pipe.rdwr);
      can_pipes_vector.push_back(new_pipe);
  }
  for(int i = 0; i<26;i++)
  {
      Pipes new_pipe;
      pipe(new_pipe.rdwr);
      processed_pipes_vector.push_back(new_pipe);
  }
  std::thread can_thread(retrieve,shrdFutureObj);
  std::thread data_proccesing_thread(set_data_processing_module,shrdFutureObj);
  std::thread profiling_thread(set_profiling_module, can_pipes_vector[PROFILING_MESSAGE_PIPE]);
  std::thread udf_thread(check_data_from_dprocess);
  can_thread.join();
  data_proccesing_thread.join();
  profiling_thread.join();
  udf_thread.join();
  return 0;
}

