#ifndef EDGEAI_FUNCTIONS_H
#define EDGEAI_FUNCTIONS_H

#include "on_board_data_interface.hpp"
#include <fdeep/fdeep.hpp>
#include <string>

extern const int CRASH_FUNCTION;

class DFLOW_Onboard_Addon_Functions{
    public:
        DFLOW_Onboard_Addon_Functions(int,std::string);
        bool crashAIfunction(OnBoardDataInterface);
    private:
        fdeep::model model;
        int type;
};

#endif