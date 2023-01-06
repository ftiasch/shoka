#include "binpow.h"
#include "universal_euclidean.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>

namespace universal_euclidean {

struct StringMonoid {
  static StringMonoid mul_id() { return {""}; }

  static StringMonoid R() { return {"R"}; }

  static StringMonoid U() { return {"U"}; }

  StringMonoid operator*(const StringMonoid &o) const { return {s + o.s}; }

  StringMonoid &operator*=(const StringMonoid &o) {
    return *this = (*this * o);
  }

  StringMonoid power(int n) const { return binpow(*this, n); }

  std::string s;
};

struct SumMonoid {
  static SumMonoid mul_id() { return {0, 0, 0}; }

  static SumMonoid R() { return {1, 0, 0}; }

  static SumMonoid U() { return {0, 1, 0}; }

  SumMonoid operator*(const SumMonoid &o) const {
    return {r + o.r, u + o.u, u_sum + u * o.r + o.u_sum};
  }

  SumMonoid &operator*=(const SumMonoid &o) { return *this = (*this * o); }

  SumMonoid power(int n) const { return binpow(*this, n); }

  uint64_t r, u, u_sum;
};

} // namespace universal_euclidean

TEST_CASE("universal_euclidean") {
  using namespace universal_euclidean;

  SECTION("string") {
    REQUIRE(UniversalEuclidean<StringMonoid>{}(5, 19, 15, 10).s ==
            "URUURUURUURUURUUR");
  }

  SECTION("sum") {
    auto c = GENERATE(range(1, 10));
    auto a = GENERATE(range(0, 10));
    auto b = GENERATE(range(0, 10));

    uint64_t answer = 0;
    for (int n = 0; n < 10; ++n) {
      answer += (a * n + b) / c;
      REQUIRE(UniversalEuclidean<SumMonoid>{}(n, a, b, c).u_sum == answer);
    }
  }
}
