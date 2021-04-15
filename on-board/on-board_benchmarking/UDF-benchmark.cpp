//Author:Radu-Tudor Andra
#include <unistd.h>
#include <string>
#include <thread>
#include <ctime>
#include <ratio>
#include <chrono>
#include <numeric>
#include <pipes.hpp>
#include "profiling_module.hpp"
#include "config.hpp"
#include <fdeep/fdeep.hpp>

using namespace std;


//set up listener to check data processing output
void set_edgeAI_tester()
{
  double sum=0;
  int index=0;
  ProfilingModule profiling_module;
  
  fdeep::model model = fdeep::load_model("./fdeep_crash_model.json");
  std::cout<<"CPU Usage is "<<profiling_module.getCPUUsage()<<std::endl;
  while(index<1000)
  {

    std::chrono::steady_clock::time_point initial_time = std::chrono::steady_clock::now();

    std::vector<float> input_values = {0.3,0.3,0.3};
    //get the model prediction
    const auto result = model.predict(
        {fdeep::tensor(fdeep::tensor_shape(static_cast<std::size_t>(3)),
        input_values)});
    //convert prediction from tensor to float
    const float result_value = fdeep::internal::to_singleton_value(fdeep::internal::single_tensor_from_tensors(result));
    std::chrono::steady_clock::time_point final_time = std::chrono::steady_clock::now();

    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(final_time - initial_time);
    double time =  time_span.count();
    sum+=time;
    index++;
  }
  std::cout<<"CPU Usage is "<<profiling_module.getCPUUsage()<<std::endl;
  std::cout<<"Total time "<<sum<<std::endl;
  std::cout<<"Average function time "<<sum/index<<std::endl;
}


void set_profiling_module()
{
 /* ProfilingModule profiling_module;
  while(true)
  {
    std::cout<<"CPU Usage is "<<profiling_module.getCPUUsage()<<std::endl;
    std::cout<<"Memory Usage is "<<profiling_module.getMemoryUsage()<<std::endl;
    usleep(1000000);
  }*/
}

int main() {

  //create exit signal to stop listeners on threads
  std::promise<void> exitSignal;
  std::future<void> futureObj = exitSignal.get_future();
  std::shared_future<void> shrdFutureObj = futureObj.share();

  //create threads
  std::thread profiling_thread(set_profiling_module);
  std::thread udf_thread(set_edgeAI_tester);

  udf_thread.join();
  profiling_thread.join();
  return 0;
}