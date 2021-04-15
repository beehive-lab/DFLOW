cmake_minimum_required(VERSION 3.12)  # CMake version check
project(DFLOW)               # Create project "simple_example"
set(CMAKE_CXX_STANDARD 17)            # Enable c++14 standard

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)


# Add main.cpp file of project root directory as source file
set(CANMODULE_SOURCE
        ../src/can_module/python_module.py
        ../src/can_module/pycan_interface.cpp
        ../src/can_module/pycan_interface.hpp
        ../src/can_module/can_interface.hpp
        ../src/can_module/file_interface.cpp
        ../src/can_module/file_interface.hpp
        ../src/can_module/can_module.cpp
        ../src/can_module/can_module.hpp
        ../src/can_module/messages/engine_sensors_message.cpp
        ../src/can_module/messages/engine_sensors_message.hpp
        ../src/can_module/messages/intake_sensors_message.cpp
        ../src/can_module/messages/intake_sensors_message.hpp
        ../src/can_module/messages/abs_module_message.cpp
        ../src/can_module/messages/abs_module_message.hpp
        ../src/can_module/messages/configurable_modes_message.cpp
        ../src/can_module/messages/configurable_modes_message.hpp
        ../src/can_module/messages/imu_sensor_message.cpp
        ../src/can_module/messages/imu_sensor_message.hpp
        ../src/can_module/messages/tpm_module_message.cpp
        ../src/can_module/messages/tpm_module_message.hpp
        ../src/can_module/messages/accelerometer_message.cpp
        ../src/can_module/messages/accelerometer_message.hpp)

set(DATA_PROCESSING_SOURCE
        ../src/data_processing_module/data_process_module.cpp
        ../src/data_processing_module/pipes.cpp
        ../src/data_processing_module/data_process_module.hpp
        ../src/data_processing_module/pipes.hpp
        )

set(UDFMODULE_SOURCE
        ../src/udf_module/on_board_data_interface.cpp
        ../src/udf_module/on_board_data_interface.hpp)

set(UDFMODULE_AI_SOURCE
        ../src/udf_module/on_board_data_interface.cpp      
        ../src/udf_module/on_board_data_interface.hpp
        ../src/udf_module/edgeAI_functions.cpp
        ../src/udf_module/edgeAI_functions.hpp)

set(PROFILING_MODULE_SOURCE
        ../src/profiling_module/profiling_module.cpp
        ../src/profiling_module/profiling_module.hpp
        ../src/profiling_module/profiling_message.hpp)

set(SOURCE_FILES
        ../src/config.cpp)
 
set(HEADER_FILES  
        ../src/config.hpp
        )
 

include_directories(../src/can_module 
                ../src/ 
                ../src/data_processing_module 
                ../src/can_module/messages 
                ../src/udf_module 
                ../src/profiling_module)
 
# Add executable target with source files listed in SOURCE_FILES variable
add_executable(CAN-Bench CAN-benchmark.cpp ${SOURCE_FILES} ${HEADER_FILES} ${CANMODULE_SOURCE} ${DATA_PROCESSING_SOURCE}
                        ${PROFILING_MODULE_SOURCE})
add_executable(DP-Bench DP-benchmark.cpp ${SOURCE_FILES} ${HEADER_FILES} ${CANMODULE_SOURCE} ${DATA_PROCESSING_SOURCE}
                        ${PROFILING_MODULE_SOURCE})
add_executable(UDF-Bench UDF-benchmark.cpp ${SOURCE_FILES} ${HEADER_FILES} ${DATA_PROCESSING_SOURCE} ${PROFILING_MODULE_SOURCE})


find_package(PythonLibs REQUIRED)
find_package(frugally-deep REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_link_libraries(CAN-Bench PRIVATE Threads::Threads PUBLIC ${PYTHON_LIBRARIES} )

target_link_libraries(DP-Bench PRIVATE Threads::Threads PUBLIC ${PYTHON_LIBRARIES} )

target_link_libraries(UDF-Bench PRIVATE Threads::Threads PUBLIC ${PYTHON_LIBRARIES} )

CONFIGURE_FILE(../src/can_module/python_module.py python_module.py COPYONLY)
CONFIGURE_FILE(../src/can_module/DFLOW.dbc DFLOW.dbc COPYONLY)
CONFIGURE_FILE(../src/udf_module/fdeep_crash_model.json fdeep_crash_model.json COPYONLY)
