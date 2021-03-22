cmake_minimum_required(VERSION 3.13)  # CMake version check
project(DFLOW)               # Create project "simple_example"
set(CMAKE_CXX_STANDARD 17)            # Enable c++14 standard



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



#SET UP PROJECT
# Add main.cpp file of project root directory as source file
set(SOURCE_FILES src/main.cpp
                 src/can_module/canmodule.cpp
                 src/can_module/dataretriever.cpp
                 libs/can_module/pstream.h
                 src/config.cpp
                 src/inter_comms/WifiComms.cpp
                 src/inter_comms/BluetoothComms.cpp)

set(HEADER_FILES src/can_module/canmodule.hpp src/can_module/dataretriever.hpp src/config.hpp
                 src/inter_comms/WifiComms.h
                 src/inter_comms/BluetoothComms.h)

include_directories( src/can_module src/ libs/can_module src/inter_comms)

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(DFLOW_OnBoard ${SOURCE_FILES} ${HEADER_FILES})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(DFLOW_OnBoard PRIVATE Threads::Threads)

find_package(OpenSSL REQUIRED)
target_link_libraries(DFLOW_OnBoard PRIVATE OpenSSL::SSL)

set(TEST_SOURCE_FILES 
                 src/can_module/canmodule.cpp
                 src/can_module/dataretriever.cpp
                 libs/can_module/pstream.h
        src/inter_comms/WifiComms.cpp
        src/inter_comms/BluetoothComms.cpp
                 src/config.cpp)

set(TEST_HEADER_FILES src/can_module/canmodule.hpp src/can_module/dataretriever.hpp src/inter_comms/WifiComms.h src/inter_comms/BluetoothComms.h src/config.hpp)



#link against gtest or gtest_main as needed. Eg
add_executable(TestCan test/canmoduletest.cpp ${TEST_SOURCE_FILES} ${TEST_HEADER_FILES})
add_executable(TestComms test/communicationstest.cpp ${TEST_SOURCE_FILES} ${TEST_HEADER_FILES})
target_link_libraries(TestCan gtest_main)
target_link_libraries(TestComms gtest_main)
target_link_libraries(TestComms gmock_main)
add_test(NAME canmoduletest COMMAND TestCan)
