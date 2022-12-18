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
  constexpr int M = 10;
  for (int c = 1; c < M; ++c) {
    for (int a = 0; a < M; ++a) {
      for (int b = 0; b < M; ++b) {
        uint64_t answer = 0;
        for (int n = 0; n < M; ++n) {
          answer += (a * n + b) / c;
          REQUIRE(universal_euclidean::sum<universal_euclidean::TestMonoid>(
                      n, a, b, c)
                      .u_sum == answer);
        }
      }
    }
  }
}
