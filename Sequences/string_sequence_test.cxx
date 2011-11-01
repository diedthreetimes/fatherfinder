#include "gtest/gtest.h"
#include "StringSequence.h"

// Tests factorial of positive numbers.
TEST(StringSequenceTest, ArgumentsConstructor) {
  StringSequence s1("hi");
  EXPECT_EQ('h', s1.current());
  EXPECT_EQ('i', s1.next());
}
