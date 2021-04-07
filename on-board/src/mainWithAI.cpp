#include <unistd.h>
#include <string>
#include <thread>
#include <ctime>
#include <numeric>
#include "can_module.hpp"
#include <pipes.hpp>
#include <WifiComms.h>
#include <BluetoothComms.h>
#include <data_process_module.hpp>
#include <boost/circular_buffer.hpp>
#include "on_board_data_interface.hpp"
#include "edgeAI_functions.hpp"
using namespace std;

//second release prototype

//vectors holding the multiple pipes
std::vector<Pipes> can_pipes_vector;
std::vector<Pipes> processed_pipes_vector;

//set can module
void retrieve(std::shared_future<void> futureObj)
{
  CAN_Module can_module = CAN_Module(std::string("./DFLOW.dbc"),std::string("notusedatthemom"),std::string("./accel_file.txt"));
  can_module.setListener(can_pipes_vector, futureObj);
}

//set data processing module
void set_data_processing_module(std::shared_future<void> futureObj)
{
  //each entry in the data modes vector determines what the output of the data processing pipe represents
  std::vector<int> data_modes{AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,
                              AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,
                              AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,
                              AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER};
  dataProcessing processing_module = dataProcessing(can_pipes_vector, processed_pipes_vector, data_modes, 1, 3, 3);
  processing_module.startProcessing(futureObj);
}

//UNUSED FOR NOW UNTIL INTER_COMS IS FINISHED
/*void inter_coms_send(int *pip)
{
  //CAN_example_message received_message = CAN_example_message();
  char ostring[200];
  WifiComms wifi_socket = WifiComms(true);
  wifi_socket.establish_connection(8080);
  char wifi_buffer[1024];
  while(true)
  {
      read(pip[0],ostring,200);
     // received_message.decodeFromPipe(ostring);
      std::string message_string = std::string();
      message_string = "Received message with data: temperature = " + std::to_string(received_message.temperature) + "; average radius = " + std::to_string(received_message.average_radius) + "; switch value = " + std::to_string(received_message.switch_value);
      wifi_socket.receive(wifi_buffer);
      if(strcmp(wifi_buffer,"Listening") == 0)
          wifi_socket.send((char*)message_string.c_str());
  }
}*/

//set up listener to check data processing output
void check_data_from_dprocess()
{
  OnBoardDataInterface data_interface(processed_pipes_vector);
  DFLOW_Onboard_Addon_Functions AI_func(CRASH_FUNCTION, "./fdeep_crash_model.json");
  while(true)
  {
    std::cout<<AI_func.crashAIfunction(data_interface)<<std::endl;
  }
}


int main() {

  //create exit signal to stop listeners on threads
  std::promise<void> exitSignal;
  std::future<void> futureObj = exitSignal.get_future();
  std::shared_future<void> shrdFutureObj = futureObj.share();
  
  //initialize can pipes and data_proccesing pipes
  for(int i = 0; i<7; i++)
  {
      Pipes new_pipe;
      pipe(new_pipe.rdwr);
      can_pipes_vector.push_back(new_pipe);
  }
  for(int i = 0; i<22;i++)
  {
      Pipes new_pipe;
      pipe(new_pipe.rdwr);
      processed_pipes_vector.push_back(new_pipe);
  }

  //create threads
  std::thread first(retrieve,shrdFutureObj);
  std::thread second(set_data_processing_module,shrdFutureObj);
  std::thread third(check_data_from_dprocess);
  first.join();
  second.join();
  third.join();
  return 0;
}
