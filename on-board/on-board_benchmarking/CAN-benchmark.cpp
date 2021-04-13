//Author:Radu-Tudor Andra
#include <unistd.h>
#include <string>
#include <thread>
#include <ctime>
#include <numeric>
#include "can_module.hpp"
#include <pipes.hpp>
#include <data_process_module.hpp>
#include <boost/circular_buffer.hpp>
#include "on_board_data_interface.hpp"
#include "profiling_module.hpp"
#include "config.hpp"

using namespace std;


//vectors holding the multiple pipes
std::vector<Pipes> can_pipes_vector;
Pipes config_pipe;

//set can module
void retrieve(std::shared_future<void> futureObj)
{
  CAN_Module can_module = CAN_Module(DFLOW_DBC_PATH,PYTHON_PATH,ACCELEROMETER_PATH);
  can_module.setListener(can_pipes_vector,config_pipe, futureObj);
}

void set_profiling_module(Pipes profiling_pipe)
{
  ProfilingModule profiling_module;
  while(true)
  {
    std::cout<<"CPU Usage is "<<profiling_module.getCPUUsage()<<std::endl;
    std::cout<<"Memory Usage is "<<profiling_module.getMemoryUsage()<<std::endl;
    usleep(1000000);
  }
}

void set_test_bench()
{
    int test_value = -1;
    while(true)
    {
        ABSModuleMessage abs_message;
        read(can_pipes_vector[ABS_MESSAGE_PIPE].rdwr[READ],&abs_message.data, sizeof(abs_message.data));
        if(test_value == -1 || test_value == 10000)
            test_value = abs_message.data.rear_wheel_speed;
        else
            if(abs_message.data.rear_wheel_speed == test_value + 10)
                test_value +=10;
            else
                break;
    }
    while(true)
    {
        std::cout<<"WARNING: Benchmark failed"<<std::endl;
        usleep(100000);
    }
}

int main() {

  //create exit signal to stop listeners on threads
  std::promise<void> exitSignal;
  std::future<void> futureObj = exitSignal.get_future();
  std::shared_future<void> shrdFutureObj = futureObj.share();

  //initialize can pipes and data_proccesing pipes
  pipe(config_pipe.rdwr);
  for(int i = 0; i<8; i++)
  {
      Pipes new_pipe;
      pipe(new_pipe.rdwr);
      can_pipes_vector.push_back(new_pipe);
  }

  //create threads
  std::thread can_thread(retrieve,shrdFutureObj);
  std::thread profiling_thread(set_profiling_module, can_pipes_vector[PROFILING_MESSAGE_PIPE]);
  std::thread testing_thread(set_test_bench);
  can_thread.join();
  profiling_thread.join();
  testing_thread.join();

  return 0;
}
