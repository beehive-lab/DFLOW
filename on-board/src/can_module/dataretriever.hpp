#include <canmodule.hpp>
#include <fstream>
#include <ctime>
#include <stdexcept>
#include <string>
#include <iostream>
#include <algorithm>
#include "canmodule.hpp"

#ifndef DATARETRIEVER_H // include guard
#define DATERETRIEVER_H


class dretriever
{
    public:
        void listentemp(int *pip);
        dretriever();
        bool assignExampleMessage(std::string);
    private:
        CAN_example_message example_message;
};
#endif
