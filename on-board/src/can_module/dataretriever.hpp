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


class dataRetriever
{
    public:
        void listen(int *pip);
        dataRetriever();
        bool testing = false;
        CAN_example_message example_message;
};
#endif
