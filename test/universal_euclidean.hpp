#include "universal_euclidean.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>

namespace universal_euclidean {

struct TestMonoid {
  static TestMonoid identity() { return TestMonoid{0, 0, 0}; }

  static TestMonoid R() { return TestMonoid{1, 0, 0}; }

  static TestMonoid U() { return TestMonoid{0, 1, 0}; }

  TestMonoid operator*(const TestMonoid &o) const {
    return TestMonoid{r + o.r, u + o.u, u_sum + u * o.r + o.u_sum};
  }

  uint64_t r, u, u_sum;
};

} // namespace universal_euclidean

TEST_CASE("universal_euclidean") {
  using namespace universal_euclidean;

  auto c = GENERATE(range(1, 10));
  auto a = GENERATE(range(0, 10));
  auto b = GENERATE(range(0, 10));

  uint64_t answer = 0;
  for (int n = 0; n < 10; ++n) {
    answer += (a * n + b) / c;
    REQUIRE(sum<TestMonoid>(n, a, b, c).u_sum == answer);
  }
}
