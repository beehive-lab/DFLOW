#ifndef EDGEAI_FUNCTIONS_H
#define EDGEAI_FUNCTIONS_H

#include "on_board_data_interface.hpp"
#include <fdeep/fdeep.hpp>
#include <string>

extern const int CRASH_FUNCTION;
extern const int FLOAT_UDF_DATA_TYPE;
extern const int INT_UDF_DATA_TYPE;
extern const int BOOL_UDF_DATA_TYPE;
class DFLOW_Onboard_Addon_Functions{
    public:
        DFLOW_Onboard_Addon_Functions();
        float AIfunction(OnBoardDataInterface, std::vector<int>,std::vector<int>, std::string);
};

#endif