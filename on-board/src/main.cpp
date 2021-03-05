#include "dataretriever.hpp"
#include <unistd.h>
#include <string>
#include <thread>
#include <numeric>
#include <canmodule.hpp>
#include "can_interface.hpp"
#include <pipes.hpp>
#include <WifiComms.h>
#include <BluetoothComms.h>
#include <data_process_module.hpp>
#include <boost/circular_buffer.hpp>
using namespace std;

//second release prototype
std::vector<Pipes> can_pipes_vector;
std::vector<Pipes> processed_pipes_vector;

void retrieve()
{
  CAN_Interface can_interface = CAN_Interface(std::string("./DFLOW.dbc"),std::string("notusedatthemom"));
  can_interface.setListener(can_pipes_vector);
}

void set_data_processing_module(std::future<void> futureObj)
{
  std::vector<int> data_modes{AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,
                              AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,DO_NOT_COMPUTE,DO_NOT_COMPUTE,DO_NOT_COMPUTE,AVERAGE_OF_BUFFER,
                              AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER};
  //dataProcessing processing_module = dataProcessing(pip,1,5,3);
  //processing_module.setPipes(pipe_vector,data_modes);
  dataProcessing processing_module = dataProcessing(can_pipes_vector, processed_pipes_vector, data_modes, 1, 3, 3);
  processing_module.startProcessing(std::move(futureObj));
}
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

void inter_coms_send_temp_from_dprocess()
{
 // WifiComms wifi_socket = WifiComms(true);
//  wifi_socket.establish_connection(8080);
  //char wifi_buffer[1024];
  float air_temperature,tyre_pressure_front,tyre_pressure_rear,lean_angle, battery_voltage,oil_pressure,water_temperature;
  int throttle_position,motorcycle_speed,rear_wheel_speed,front_wheel_speed,rear_brake_active,front_brake_active,abs_mode,tc_mode,throttle_response_mode,gear_position,engine_speed;
  //for(int i=0;i<18;i++)
  //      int retval = fcntl(processed_pipes_vector[i].rdwr[READ],F_SETFL,fcntl(processed_pipes_vector[i].rdwr[READ],F_GETFL) | O_NONBLOCK);
  while(true)
  {
      //std::ostringstream oss_temp;
      //std::ostringstream oss_radius;
      //std::ostringstream oss_switch;
      read(processed_pipes_vector[AIR_TEMPERATURE_PIPE].rdwr[READ],&air_temperature,sizeof(float));
      std::cout<<"air temp "<<air_temperature<<std::endl;
      read(processed_pipes_vector[THROTTLE_POSITION_PIPE].rdwr[READ],&throttle_position,sizeof(int));
      std::cout<<"throttle pos "<<throttle_position<<std::endl;
      read(processed_pipes_vector[TYRE_PRESSURE_FRONT_PIPE].rdwr[READ],&tyre_pressure_front,sizeof(float));
      std::cout<<"tyre pres fr "<<tyre_pressure_front<<std::endl;
      read(processed_pipes_vector[TYRE_PRESSURE_REAR_PIPE].rdwr[READ],&tyre_pressure_rear,sizeof(float));
      std::cout<<"tyre pres re "<<tyre_pressure_rear<<std::endl;
      read(processed_pipes_vector[MOTORCYCLE_SPEED_PIPE].rdwr[READ],&motorcycle_speed,sizeof(int));
      std::cout<<"moto speed "<<motorcycle_speed<<std::endl;
      read(processed_pipes_vector[REAR_WHEEL_SPEED_PIPE].rdwr[READ],&rear_wheel_speed,sizeof(int));
      std::cout<<"rear whe speed "<<rear_wheel_speed<<std::endl;
      read(processed_pipes_vector[FRONT_WHEEL_SPEED_PIPE].rdwr[READ],&front_wheel_speed,sizeof(int));
      std::cout<<"front whe speed "<<front_wheel_speed<<std::endl;
      read(processed_pipes_vector[BRAKE_REAR_ACTIVE_PIPE].rdwr[READ],&rear_brake_active,sizeof(int));
      std::cout<<"brake fr active "<<front_brake_active<<std::endl;
      //read(processed_pipes_vector[BRAKE_FRONT_ACTIVE_PIPE].rdwr[READ],&front_brake_active,sizeof(int));
      //std::cout<<"brake re active "<<rear_brake_active<<std::endl;
      /*//read(processed_pipes_vector[ABS_MODE_PIPE].rdwr[READ],&abs_mode,sizeof(int));
      //read(processed_pipes_vector[TC_MODE_PIPE].rdwr[READ],&tc_mode,sizeof(int));
      //read(processed_pipes_vector[THROTTLE_RESPONSE_MODE_PIPE].rdwr[READ],&throttle_response_mode,sizeof(int));
      read(processed_pipes_vector[LEAN_ANGLE_PIPE].rdwr[READ],&lean_angle,sizeof(float));
      std::cout<<"lean angle "<<lean_angle<<std::endl;
      read(processed_pipes_vector[BATTERY_VOLTAGE_PIPE].rdwr[READ],&battery_voltage,sizeof(float));
      std::cout<<"battery volt "<<battery_voltage<<std::endl;
      read(processed_pipes_vector[OIL_PRESSURE_PIPE].rdwr[READ],&oil_pressure,sizeof(float));
      std::cout<<"oil pres "<<oil_pressure<<std::endl;
      read(processed_pipes_vector[GEAR_POSITION_PIPE].rdwr[READ],&gear_position,sizeof(int));
      std::cout<<"gear pos "<<gear_position<<std::endl;
      read(processed_pipes_vector[WATER_TEMPERATURE_PIPE].rdwr[READ],&water_temperature,sizeof(float));
      std::cout<<"water temp "<<water_temperature<<std::endl;
      read(processed_pipes_vector[ENGINE_SPEED_PIPE].rdwr[READ],&engine_speed,sizeof(int));
      
      //std::cout<<"abs mode "<<abs_mode<<std::endl;
      //std::cout<<"tc mode "<<tc_mode<<std::endl;
      //std::cout<<"tr resp mode "<<throttle_response_mode<<std::endl;
      std::cout<<"eng speed "<<engine_speed<<std::endl;
*/
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

  std::promise<void> exitSignal;
  std::future<void> futureObj = exitSignal.get_future();
  
  for(int i = 0; i<6; i++)
  {
      Pipes new_pipe;
      pipe(new_pipe.rdwr);
      can_pipes_vector.push_back(new_pipe);
  }
  for(int i = 0; i<18;i++)
  {
      Pipes new_pipe;
      pipe(new_pipe.rdwr);
      processed_pipes_vector.push_back(new_pipe);
  }

  std::thread first(retrieve);
  std::thread second(set_data_processing_module,std::move(futureObj));
  std::thread third(inter_coms_send_temp_from_dprocess);
  first.join();
  second.join();
  third.join();
   return 0;
}
