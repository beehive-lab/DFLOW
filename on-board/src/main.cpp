#include "dataretriever.hpp"
#include <unistd.h>
#include <string>
#include <thread>
#include <numeric>
#include <canmodule.hpp>
#include <pipes.hpp>
#include <WifiComms.h>
#include <BluetoothComms.h>
#include <data_process_module.hpp>
#include <boost/circular_buffer.hpp>
using namespace std;

//second release prototype
std::vector<Pipes> pipe_vector;
void retrieve(int *pip)
{
  dataRetriever retriever = {};
  retriever.listen(pip);
}
void set_data_processing_module(int *pip)
{
  std::vector<int> data_modes{SUM_OF_BUFFER,SUM_OF_BUFFER,AVERAGE_OF_BUFFER};
  dataProcessing processing_module = dataProcessing(pip,1,5,3);
  processing_module.setPipes(pipe_vector,data_modes);
}
void inter_coms_send(int *pip)
{
  CAN_example_message received_message = CAN_example_message();
  char ostring[200];
  WifiComms wifi_socket = WifiComms(true);
  wifi_socket.establish_connection(8080);
  char wifi_buffer[1024];
  while(true)
  {
      read(pip[0],ostring,200);
      received_message.decodeFromPipe(ostring);
      std::string message_string = std::string();
      message_string = "Received message with data: temperature = " + std::to_string(received_message.temperature) + "; average radius = " + std::to_string(received_message.average_radius) + "; switch value = " + std::to_string(received_message.switch_value);
      wifi_socket.receive(wifi_buffer);
      if(strcmp(wifi_buffer,"Listening") == 0)
          wifi_socket.send((char*)message_string.c_str());
  }
}
void inter_coms_send_temp_from_dprocess()
{
  //boost::circular_buffer<float> temperature_buffer;
  float temperature_sum;
  float average_radius_sum;
  bool switch_average;
  float temperature_buffer_linearized[3];
  float average_radius_buffer_linearized[3];
  bool switch_buffer_linearized[3];
 // WifiComms wifi_socket = WifiComms(true);
//  wifi_socket.establish_connection(8080);
  //char wifi_buffer[1024];
  while(true)
  {
      std::ostringstream oss_temp;
      std::ostringstream oss_radius;
      std::ostringstream oss_switch;
      read(pipe_vector[TEMPERATURE_PIPE].rdwr[READ],&temperature_sum,sizeof(float));
      read(pipe_vector[AVERAGE_RADIUS_PIPE].rdwr[READ],&average_radius_sum,sizeof(float));
      read(pipe_vector[SWITCH_PIPE].rdwr[READ],&switch_average,sizeof(bool));
      std::cout<<temperature_sum<<" "<<average_radius_sum<<" "<<switch_average<<std::endl;
      /*std::copy(std::begin(temperature_buffer_linearized), std::end(temperature_buffer_linearized),
          std::ostream_iterator<float>(oss_temp, ";"));
      std::string message_string = std::string();
      message_string = "Received message with data: temperature = " + oss_temp.str();
      std::copy(std::begin(average_radius_buffer_linearized), std::end(average_radius_buffer_linearized),
          std::ostream_iterator<float>(oss_radius, ";"));
      message_string = message_string + " average radius: " + oss_radius.str();
      std::copy(std::begin(switch_buffer_linearized), std::end(switch_buffer_linearized),
          std::ostream_iterator<bool>(oss_switch, ";"));
      message_string = message_string + " switch: " + oss_switch.str();
      std::cout<<message_string<< std::endl;
   //   wifi_socket.receive(wifi_buffer);
   //   if(strcmp(wifi_buffer,"Listening") == 0)
    //      wifi_socket.send((char*)message_string.c_str());*/
  }
}
int main() {
  for(int i = 0; i<4; i++)
  {
      Pipes new_pipe;
      pipe(new_pipe.rdwr);
      pipe_vector.push_back(new_pipe);
  }
  std::thread first(retrieve,pipe_vector[CAN_OUTPUT_PIPE].rdwr);
  std::thread second(set_data_processing_module,pipe_vector[CAN_OUTPUT_PIPE].rdwr);
  std::thread third(inter_coms_send_temp_from_dprocess);
  first.join();
  second.join();
  third.join();
   return 0;
}
