#include "../universal_euclidean.h"

#include "gtest/gtest.h"

namespace {

static const int M = 50;

struct Monoid {
  static Monoid identity() { return Monoid{0, 0, 0}; }

  static Monoid R() { return Monoid{1, 0, 0}; }

  static Monoid U() { return Monoid{0, 1, 0}; }

  Monoid operator*(const Monoid &o) const {
    return Monoid{r + o.r, u + o.u, u_sum + u * o.r + o.u_sum};
  }

  uint64_t r, u, u_sum;
};

TEST(UniversalEuclidean, Test) {
  for (int c = 1; c < M; ++c) {
    for (int a = 0; a < M; ++a) {
      for (int b = 0; b < M; ++b) {
        uint64_t answer = 0;
        for (int n = 0; n < M; ++n) {
          answer += (a * n + b) / c;
          ASSERT_EQ(answer, universal_euclidean::sum<Monoid>(n, a, b, c).u_sum);
        }
      }
    }
  }
}

} // namespace
