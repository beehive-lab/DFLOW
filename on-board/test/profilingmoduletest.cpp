#include "gtest/gtest.h"
#include <unistd.h>
#include <fcntl.h>
#include "profiling_module.hpp"
#include "profiling_message.hpp"
#include "pipes.hpp"

namespace {

class ProfilingModuleTest : public ::testing::Test {

};


//test listener
TEST_F(ProfilingModuleTest, TestCPUUsage) {
    ProfilingModule profiling_module;
    EXPECT_FLOAT_EQ(profiling_module.getCPUUsage(), -1);
    usleep(100000);
    ASSERT_NE(profiling_module.getCPUUsage(), -1);
}

TEST_F(ProfilingModuleTest, TestMemUsage) {
    ProfilingModule profiling_module;
    ASSERT_GT(profiling_module.getMemoryUsage(), 0);
}

TEST_F(ProfilingModuleTest, TestCPUTemp) {
    ProfilingModule profiling_module;
    ASSERT_GT(profiling_module.getCPUTemperature(), 0);
}

TEST_F(ProfilingModuleTest, TestCPUFreq) {
    ProfilingModule profiling_module;
    ASSERT_GT(profiling_module.getCPUFrequency(), 0);
}

TEST_F(ProfilingModuleTest, TestComputeAndSend) {
    Pipes test_pipe;
    pipe(test_pipe.rdwr);
    ProfilingModule profiling_module;
    ProfilingMessage profiling_message;
    int retval = fcntl(test_pipe.rdwr[READ],F_SETFL,fcntl(test_pipe.rdwr[READ],F_GETFL) | O_NONBLOCK);
    profiling_module.computeAndSendStats(test_pipe);
    ssize_t message_r = read(test_pipe.rdwr[READ], &profiling_message.data, sizeof(profiling_message.data));
    ASSERT_GT(message_r,0);
}

}