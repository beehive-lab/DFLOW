//Author: Radu-Tudor Andra
#include "pycan_interface.hpp"
#include <string>
#include <pipes.hpp>
#include "gtest/gtest.h"
#include "gmock/gmock.h"

namespace{

class PythonInterfaceTest : public ::testing::Test {

};

TEST_F(PythonInterfaceTest,TestPythonInitialisation)
{
  CAN_Python_Interface python_interface;
  EXPECT_FALSE(python_interface.checkClassInstantiated());
  python_interface.initializeInterface(std::string("./DFLOW.dbc"),std::string("test"));
  EXPECT_TRUE(python_interface.checkClassInstantiated());
}

}  // namespace