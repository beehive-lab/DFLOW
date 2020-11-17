cmake_minimum_required(VERSION 3.13)  # CMake version check
project(DFLOW)               # Create project "simple_example"
set(CMAKE_CXX_STANDARD 14)            # Enable c++14 standard

# Add main.cpp file of project root directory as source file
set(SOURCE_FILES src/main.cpp
        src/can_module/canmodule.c
        src/inter_comms/WifiComms.cpp)

set(HEADER_FILES src/can_module/canmodule.h
        src/inter_comms/WifiComms.h)


include_directories(src/can_module
        src/inter_comms)

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(DFLOW_OnBoard ${SOURCE_FILES} ${HEADER_FILES})