cmake_minimum_required(VERSION 3.12)  # CMake version check
project(DFLOW)               # Create project "simple_example"
set(CMAKE_CXX_STANDARD 17)            # Enable c++14 standard

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)

#SET UP GOOGLETEST
# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
 RESULT_VARIABLE result
 WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
 message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
 RESULT_VARIABLE result
 WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
 message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
 include_directories("${gtest_SOURCE_DIR}/include")
endif()

#add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/../src/udf_module)

#SET UP PROJECT

#execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/src/udf_module/install_frugally-deep.sh)

# Add main.cpp file of project root directory as source file
set(CANMODULE_SOURCE
        src/can_module/python_module.py
        src/can_module/pycan_interface.cpp
        src/can_module/pycan_interface.hpp
        src/can_module/can_interface.hpp
        src/can_module/file_interface.cpp
        src/can_module/file_interface.hpp
        src/can_module/can_module.cpp
        src/can_module/can_module.hpp
        src/can_module/messages/engine_sensors_message.cpp
        src/can_module/messages/engine_sensors_message.hpp
        src/can_module/messages/intake_sensors_message.cpp
        src/can_module/messages/intake_sensors_message.hpp
        src/can_module/messages/abs_module_message.cpp
        src/can_module/messages/abs_module_message.hpp
        src/can_module/messages/configurable_modes_message.cpp
        src/can_module/messages/configurable_modes_message.hpp
        src/can_module/messages/imu_sensor_message.cpp
        src/can_module/messages/imu_sensor_message.hpp
        src/can_module/messages/tpm_module_message.cpp
        src/can_module/messages/tpm_module_message.hpp
        src/can_module/messages/accelerometer_message.cpp
        src/can_module/messages/accelerometer_message.hpp)

set(DATA_PROCESSING_SOURCE
        src/data_processing_module/data_process_module.cpp
        src/data_processing_module/pipes.cpp
        src/data_processing_module/data_process_module.hpp
        src/data_processing_module/pipes.hpp
        )

set(UDFMODULE_SOURCE
        src/udf_module/on_board_data_interface.cpp
        src/udf_module/on_board_data_interface.hpp)

set(UDFMODULE_AI_SOURCE
        src/udf_module/on_board_data_interface.cpp      
        src/udf_module/on_board_data_interface.hpp
        src/udf_module/edgeAI_functions.cpp
        src/udf_module/edgeAI_functions.hpp)

set(PROFILING_MODULE_SOURCE
        src/profiling_module/profiling_module.cpp
        src/profiling_module/profiling_module.hpp
        src/profiling_module/profiling_message.hpp)

set(INTER_COMMS_SOURCE
        src/inter_comms/WifiComms.cpp
        src/inter_comms/BluetoothComms.cpp
        src/inter_comms/Logic.cpp
        src/inter_comms/BluetoothLogic.cpp
        src/inter_comms/WifiComms.h
        src/inter_comms/BluetoothComms.h
        src/inter_comms/Logic.h
        src/inter_comms/BluetoothLogic.h
        )

set(SECURITY_SOURCE
        src/crypto/SymmetricEncryption.cpp
        src/crypto/SymmetricEncryption.h
        )

set(SOURCE_FILES
        src/main.cpp
        src/config.cpp)
 
set(HEADER_FILES  
        src/config.hpp
        )
 
set(AI_SOURCE_FILES
        src/mainWithAI.cpp
        src/config.cpp)

include_directories(src/can_module 
                src/ 
                src/inter_comms 
                src/data_processing_module 
                src/can_module/messages 
                src/udf_module 
                src/profiling_module)
 
# Add executable target with source files listed in SOURCE_FILES variable
add_executable(DFLOW_OnBoard  ${SOURCE_FILES} ${HEADER_FILES} ${CANMODULE_SOURCE} ${DATA_PROCESSING_SOURCE}
                                 ${UDFMODULE_SOURCE} ${PROFILING_MODULE_SOURCE} ${INTER_COMMS_SOURCE} ${SECURITY_SOURCE})

add_executable(DFLOW_OnBoard_WithAI ${AI_SOURCE_FILES} ${HEADER_FILES} ${CANMODULE_SOURCE} ${DATA_PROCESSING_SOURCE}
                                ${UDFMODULE_AI_SOURCE} ${PROFILING_MODULE_SOURCE} ${INTER_COMMS_SOURCE} ${SECURITY_SOURCE})

find_package(PythonLibs REQUIRED)
find_package(frugally-deep REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(OpenSSL REQUIRED)
link_libraries(ssl crypto)
target_link_libraries(DFLOW_OnBoard PRIVATE Threads::Threads OpenSSL::SSL PUBLIC ${PYTHON_LIBRARIES} )
include_directories(${PYTHON_INCLUDE_DIRS})
target_link_libraries(DFLOW_OnBoard_WithAI PRIVATE Threads::Threads OpenSSL::SSL PUBLIC ${PYTHON_LIBRARIES} )
 
# testing set up
set(CAN_TEST_FILES
                        src/can_module/pycan_interface.hpp
                        src/can_module/can_interface.hpp
                        src/can_module/can_module.hpp
                        src/can_module/messages/intake_sensors_message.hpp
                        src/config.hpp
                        src/data_processing_module/data_process_module.hpp
                        src/data_processing_module/pipes.hpp
                        src/data_processing_module/pipes.cpp)
 
set(DATAP_TEST_FILES 
                        src/config.cpp
                        src/data_processing_module/data_process_module.cpp
                        src/data_processing_module/pipes.cpp
                        src/inter_comms/WifiComms.h
                        src/inter_comms/BluetoothComms.h
                        src/config.hpp
                        src/data_processing_module/data_process_module.hpp
                        src/data_processing_module/pipes.hpp
                        src/can_module/messages/engine_sensors_message.cpp
                        src/can_module/messages/engine_sensors_message.hpp
                        src/can_module/messages/intake_sensors_message.cpp
                        src/can_module/messages/intake_sensors_message.hpp
                        src/can_module/messages/abs_module_message.cpp
                        src/can_module/messages/abs_module_message.hpp
                        src/can_module/messages/configurable_modes_message.cpp
                        src/can_module/messages/configurable_modes_message.hpp
                        src/can_module/messages/imu_sensor_message.cpp
                        src/can_module/messages/imu_sensor_message.hpp
                        src/can_module/messages/tpm_module_message.cpp
                        src/can_module/messages/tpm_module_message.hpp)

set(COMMS_TEST_FILES
                        src/config.cpp
                        src/inter_comms/WifiComms.h
                        src/inter_comms/BluetoothComms.h
                        src/config.hpp
                        src/inter_comms/WifiComms.cpp
                        src/inter_comms/BluetoothComms.cpp
)

set(CRYPTO_TEST_FILES
                        src/config.cpp
                        src/config.hpp
                        src/crypto/SymmetricEncryption.h
                        src/crypto/SymmetricEncryption.cpp
)
 
set(UDF_TEST_FILES
                        src/udf_module/on_board_data_interface.cpp
                        src/udf_module/on_board_data_interface.hpp
                        src/udf_module/edgeAI_functions.cpp
                        src/udf_module/edgeAI_functions.hpp
                        src/data_processing_module/pipes.cpp
                        src/data_processing_module/pipes.hpp)
 
set(PROFILING_TEST_FILES
                        src/profiling_module/profiling_module.cpp
                        src/profiling_module/profiling_module.hpp
                        src/profiling_module/profiling_message.hpp
                        src/data_processing_module/pipes.hpp
                        src/data_processing_module/pipes.cpp)
 

#link against gtest or gtest_main as needed. Eg
add_executable(TestCan test/canmoduletest.cpp ${CAN_TEST_FILES} ${CANMODULE_SOURCE})
add_executable(TestPythonInterface test/pythoninterfacetest.cpp ${CAN_TEST_FILES} ${CANMODULE_SOURCE})
add_executable(TestComms test/communicationstest.cpp ${COMMS_TEST_FILES})
add_executable(TestDataProcessing test/dataprocessingtest.cpp ${DATAP_TEST_FILES})
add_executable(TestUDF test/udfmoduletest.cpp ${UDF_TEST_FILES})
add_executable(TestCrypto test/cryptotest.cpp ${CRYPTO_TEST_FILES})
add_executable(TestProfiling test/profilingmoduletest.cpp ${PROFILING_TEST_FILES})
target_link_libraries(TestCan PUBLIC gtest_main gmock_main ${PYTHON_LIBRARIES})
target_link_libraries(TestPythonInterface PUBLIC gtest_main gmock_main ${PYTHON_LIBRARIES})
target_link_libraries(TestComms PUBLIC gtest_main gmock_main OpenSSL::SSL)
target_link_libraries(TestDataProcessing PUBLIC gtest_main gmock_main)
target_link_libraries(TestUDF PUBLIC gtest_main gmock_main)
target_link_libraries(TestCrypto PUBLIC gtest_main OpenSSL::SSL)
target_link_libraries(TestProfiling PUBLIC gtest_main)



CONFIGURE_FILE(src/can_module/python_module.py python_module.py COPYONLY)
CONFIGURE_FILE(src/can_module/DFLOW.dbc DFLOW.dbc COPYONLY)
CONFIGURE_FILE(src/udf_module/fdeep_crash_model.json fdeep_crash_model.json COPYONLY)
