#include <string>
#include <map>

#ifndef CAN_Interface_H
#define CAN_Interface_H

//interface for the CAN communciation - used mainly for replacing the current interface with a mock interface when testing
class CAN_Interface{
    public:
        virtual void instantiatePythonClass(std::string, std::string) = 0;
        virtual std::map<std::string, std::string> getMessageFromPythonModule() = 0;
        virtual void sendMessage(std::map<std::string,std::string>) = 0;
        virtual ~CAN_Interface(){};
};

#endif