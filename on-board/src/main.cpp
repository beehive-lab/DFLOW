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
using namespace std;

//second release prototype

//vectors holding the multiple pipes
std::vector<Pipes> can_pipes_vector;
std::vector<Pipes> processed_pipes_vector;

//set can module
void retrieve(std::shared_future<void> futureObj)
{
  CAN_Module can_module = CAN_Module(std::string("./DFLOW.dbc"),std::string("notusedatthemom"));
  can_module.setListener(can_pipes_vector, futureObj);
}

//set data processing module
void set_data_processing_module(std::shared_future<void> futureObj)
{
  //each entry in the data modes vector determines what the output of the data processing pipe represents
  std::vector<int> data_modes{AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,
                              AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,DO_NOT_COMPUTE,DO_NOT_COMPUTE,DO_NOT_COMPUTE,
                              AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,AVERAGE_OF_BUFFER,
                              AVERAGE_OF_BUFFER};
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
  //values
  float air_temperature,tyre_pressure_front,tyre_pressure_rear,lean_angle, battery_voltage,oil_pressure,water_temperature;
  int throttle_position,motorcycle_speed,rear_wheel_speed,front_wheel_speed,rear_brake_active,front_brake_active,abs_mode,tc_mode,throttle_response_mode,gear_position,engine_speed;
  time_t time_recveived;

  //FOR NON_BLOCKING READS
  //for(int i=0;i<18;i++)
  //      int retval = fcntl(processed_pipes_vector[i].rdwr[READ],F_SETFL,fcntl(processed_pipes_vector[i].rdwr[READ],F_GETFL) | O_NONBLOCK);
 
  while(true)
  {
      read(processed_pipes_vector[TIMESTAMP_PIPE].rdwr[READ],&time_recveived,sizeof(time_t));
      std::cout<<std::endl<<"MESSAGES AT TIMESTAMP: "<<ctime(&time_recveived)<<std::endl;
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
      std::cout<<"brake re active "<<rear_brake_active<<std::endl;
      read(processed_pipes_vector[BRAKE_FRONT_ACTIVE_PIPE].rdwr[READ],&front_brake_active,sizeof(int));
      std::cout<<"brake fr active "<<front_brake_active<<std::endl;
      //DISABLED BECAUSE THERE ARE NO SENT CONFIG MESSAGES
      //read(processed_pipes_vector[ABS_MODE_PIPE].rdwr[READ],&abs_mode,sizeof(int));
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

  }
}


int main() {

  //create exit signal to stop listeners on threads
  std::promise<void> exitSignal;
  std::future<void> futureObj = exitSignal.get_future();
  std::shared_future<void> shrdFutureObj = futureObj.share();
  
  //initialize can pipes and data_proccesing pipes
  for(int i = 0; i<6; i++)
  {
      Pipes new_pipe;
      pipe(new_pipe.rdwr);
      can_pipes_vector.push_back(new_pipe);
  }
  for(int i = 0; i<19;i++)
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
