#include "dataretriever.hpp"
#include <unistd.h>
#include <thread>
#include <canmodule.hpp>
#include <WifiComms.h>
#include <BluetoothComms.g>

using namespace std;


void retrieve(int *pip)
{
    dataRetriever retriever = {};
    retriever.listen(pip);
}

void listen(int *pip)
{
    CAN_example_message received_message = CAN_example_message();
    char ostring[200];
    while(true)
    {
        read(pip[0],ostring,200);
        received_message.decodeFromPipe(ostring);

        std::cout<<"From main: "<<received_message.temperature<<std::endl;
    }
}

int main() {
    int pip[2];

    pipe(pip);

    std::thread first(retrieve,pip);
    std::thread second(listen,pip);

    first.join();
    second.join();
    
    return 0;
}