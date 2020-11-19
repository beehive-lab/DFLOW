extern "C"{
    #include "canmodule.h"
}
#include "gtest/gtest.h"
namespace {


TEST(EnableValueInRangeTest, InBounds) {
  EXPECT_TRUE(canmodule_example_message_enable_is_in_range(0));
  EXPECT_TRUE(canmodule_example_message_enable_is_in_range(1));
}

TEST(EnableValueInRangeTest, OutOfBounds) {
  ASSERT_FALSE(canmodule_example_message_enable_is_in_range(200));
}

}  // namespace