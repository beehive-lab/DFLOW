#include "dataretriever.hpp"
#include "config.hpp"
#include <ext/stdio_filebuf.h>
#include <canmodule.hpp>
#include <fcntl.h>
#include "pstream.h"
#include <string> 
#include <iostream> 
#include <fstream>
#include <exception>
#include <ios>


//constructor
dataRetriever::dataRetriever()
{
  dataRetriever::example_message = CAN_example_message();
  dataRetriever::example_message.reset();
}

//listens for oncoming messages over vcan0 and encodes them
void dataRetriever::listen(int *pip)
{
  //process stream to dump contents of vcan0
  redi::ipstream candump_stream("candump vcan0"); 

  std::string candump_line;
  
  int message_type;

  if(!candump_stream)
  {
    std::cerr<<"Error accessing candump proccess stream"<<std::endl;
  }

  //while there is input from candump
  while(std::getline(candump_stream, candump_line))
  {
    //protocol for testing to replace candump output with a set message
    if(testing)
    {
      candump_line = "  vcan0  000001F0   [8]  40 01 40 00 00 00 00 00";
    }

    //process stream to decode message from vcan0 using cantools
    redi::ipstream cantools_stream("echo '" + candump_line + "' | cantools decode " + dbc_file_path );

    if(!cantools_stream)
    {
      std::cerr<<"Error accessing cantools proccess stream"<<std::endl;
    }
    std::string cantools_line;

    //while there is input from cantools
    while(std::getline(cantools_stream, cantools_line))
    {
      //look for message types
      //at the moment, we only have example_messages
      if(cantools_line.find("ExampleMessage") != std::string::npos)
      {
        message_type = 0;
      }
      
      if(message_type == 0)
      {
        //add lines to the example_message structure to populate values
        example_message.addMessageLine(cantools_line);
        //once message is ready, encode it and send it over the pipe
        if(example_message.messageReady())
        { 
          write(pip[1], example_message.encodeForPipe(), 100);
          example_message.reset();
          break;
        }
      }
    }
    //protocol for testing to exist listener after one message
    if(testing)
    {
      break;
    }
  }
}
