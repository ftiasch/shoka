#include "zip_with.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>

TEST_CASE("zip_with") {
  SECTION("zip2_not_same") {
    REQUIRE_FALSE(
        zip_with_details::are_same_v<std::tuple<int>, std::tuple<int, int>>);
  }

  using T = std::tuple<int, std::string>;

  SECTION("zip1_with") {
    auto result = zip_with([]<typename T>(T x) { return x; }, T{1, "2"});
    REQUIRE(result == T{1, "2"});
  }

  SECTION("zip2_with") {
    auto result = zip_with([]<typename T>(T x, T y) { return x + y; },
                           T{1, "2"}, T{2, "33"});
    REQUIRE(result == T{3, "233"});
  }

  SECTION("zip2_with_cv") {
    const T x{1, "2"};
    auto result =
        zip_with([]<typename T>(T x, T y) { return x + y; }, x, T{2, "33"});
    REQUIRE(result == T{3, "233"});
  }

  SECTION("zip3_with") {
    auto result = zip_with([]<typename T>(T x, T y, T z) { return x + y + z; },
                           T{1, "2"}, T{2, "a"}, T{3, "b"});
    REQUIRE(result == T{6, "2ab"});
  }
}
