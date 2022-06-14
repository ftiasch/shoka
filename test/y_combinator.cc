#include "../y_combinator.h"

#include "gtest/gtest.h"

TEST(YCombinator, Factorial) {
  // NOTE: the return value declaration is essential
  auto factorial = y_combinator([](auto factorial, int n) -> int {
    return n ? factorial(n - 1) * n : 1;
  });
  ASSERT_EQ(factorial(0), 1);
  ASSERT_EQ(factorial(5), 120);
}
