#include "dataretriever.hpp"
#include <ext/stdio_filebuf.h>
#include <canmodule.hpp>
#include <fcntl.h>
#include "pstream.h"
#include <string> 
#include <iostream> 
#include <fstream>
#include <exception>
#include <ios>

bool dretriever::listenExampleMessage(std::string cantools_line)
{
  if(cantools_line.find("Enable") != std::string::npos)
  {
    dretriever::example_message.setEnable(cantools_line);
  }
  if(cantools_line.find("AverageRadius") != std::string::npos)
  {
    dretriever::example_message.setRadius(cantools_line);
  }
  if(cantools_line.find("Temperature") != std::string::npos)
  {
    dretriever::example_message.setTemp(cantools_line);
  }
  if(dretriever::example_message.messageReady())
  {
    return true;
  }
  return false;
}

void dretriever::listentemp(int *pip){
  
  dretriever::example_message = CAN_example_message();
  dretriever::example_message.reset();

  redi::ipstream candump_stream("candump vcan0"); 
  std::string candump_line;
  
  int message_type;

  if(!candump_stream)
    std::cerr<<"Error accessing candump proccess stream"<<std::endl;

  while(std::getline(candump_stream, candump_line))
  {
    redi::ipstream cantools_stream("echo '" + candump_line + "' | cantools decode /home/randra/Development/cantools-master/tests/files/dbc/motohawk.dbc ");

    if(!cantools_stream)
      std::cerr<<"Error accessing cantools proccess stream"<<std::endl;
    std::string cantools_line;

    while(std::getline(cantools_stream, cantools_line))
    {
      if(cantools_line.find("ExampleMessage") != std::string::npos)
        message_type = 0;
      
      if(message_type == 0)
        if(listenExampleMessage(cantools_line))
        { 
          write(pip[1], dretriever::example_message.encodeForPipe(), 100);
          dretriever::example_message.reset();
          break;
        }
    }
  }
}
