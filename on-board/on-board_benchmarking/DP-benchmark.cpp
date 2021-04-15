//Author:Radu-Tudor Andra
#include <unistd.h>
#include <string>
#include <thread>
#include <ctime>
#include <numeric>
#include "can_module.hpp"
#include <pipes.hpp>
#include <data_process_module.hpp>
#include "on_board_data_interface.hpp"
#include "intake_sensors_message.hpp"
#include "profiling_module.hpp"
#include "config.hpp"

using namespace std;

//second release prototype

float testing_value;
//vectors holding the multiple pipes
std::vector<Pipes> can_pipes_vector;
std::vector<Pipes> processed_pipes_vector;


//set data processing module
void set_data_processing_module(std::shared_future<void> futureObj)
{
  //each entry in the data modes vector determines what the output of the data processing pipe represents
  std::vector<int> data_modes{AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,
                              AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,
                              AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,
                              AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,
                              AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER};
  dataProcessing processing_module = dataProcessing(can_pipes_vector, processed_pipes_vector, data_modes, 0, 10, 10000);
  processing_module.startProcessing(futureObj);
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

void set_dp_tester()
{ 
  time_t last_time = time(0);
  IntakeSensorsMessage test_message;
  float air_temperature;
  testing_value = 0;
  float sum = 0;
  int times = 0;
  while(true)
  {
    if(time(0))
    test_message.data.air_temperature = testing_value;
    sum += testing_value;
    times += 1;
    testing_value +=1;
    test_message.data.throttle_position = 0;
    write(can_pipes_vector[INTAKE_MESSAGE].rdwr[WRITE],&test_message.data,sizeof(test_message.data));
    usleep(2000);
    if(difftime(time(0),last_time) >= 10)
    {
      read(processed_pipes_vector[AIR_TEMPERATURE].rdwr[READ],&air_temperature,sizeof(float));
      std::cout<<air_temperature<<" "<<sum/times<<std::endl;
      if(air_temperature == sum/times)
        std::cout<<"TEST SUCCESFULL"<<std::endl;
      else 
        std::cout<<"TEST FAILED"<<std::endl;
    }
  }
}
int main() {

  //create exit signal to stop listeners on threads
  std::promise<void> exitSignal;
  std::future<void> futureObj = exitSignal.get_future();
  std::shared_future<void> shrdFutureObj = futureObj.share();

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
  std::thread data_proccesing_thread(set_data_processing_module,shrdFutureObj);
  std::thread profiling_thread(set_profiling_module, can_pipes_vector[PROFILING_MESSAGE]);
  std::thread testing_thread(set_dp_tester);

  data_proccesing_thread.join();
  profiling_thread.join();
  testing_thread.join();

  return 0;
}