#include "y_combinator.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>

TEST_CASE("y_combinator") {
  // NOTE: the return value declaration is essential
  auto factorial = y_combinator([](auto factorial, int n) -> int {
    return n ? factorial(n - 1) * n : 1;
  });
  REQUIRE(factorial(0) == 1);
  REQUIRE(factorial(5) == 120);
}
