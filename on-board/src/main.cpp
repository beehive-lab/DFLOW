#include "dataretriever.hpp"
#include <unistd.h>
#include <string>
#include <thread>
#include <canmodule.hpp>
#include <WifiComms.h>
#include <BluetoothComms.h>

using namespace std;

//first release prototype
void retrieve(int *pip)
{
    dataRetriever retriever = {};
    retriever.listen(pip);
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

int main() {
    int pip[2];

    pipe(pip);

    std::thread first(retrieve,pip);
    std::thread second(inter_coms_send,pip);

    first.join();
    second.join();

    return 0;
}