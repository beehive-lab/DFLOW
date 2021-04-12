//Author: Radu-Tudor Andra
#include <map>
#include <string>

#ifndef TPM_MODULE_MESSAGE_H
#define TPM_MODULE_MESSAGE_H

class TPMModuleMessage{

    public:
        struct{
            float tyre_pressure_rear;
            
            float tyre_pressure_front;
        }data;

        void set_from_map(std::map<std::string,std::string>);

};

#endif