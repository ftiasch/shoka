#include "nd_array.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>

TEST_CASE("nd_array") {
  SECTION("NDArray<int, 2>") {
    auto a = NDArray<int, 2>{};
    REQUIRE(a.size() == 2);
  }

  SECTION("NDArray<int, 2, 3>") {
    auto a = NDArray<int, 2, 3>{};
    REQUIRE(a.size() == 2);
    REQUIRE(a[0].size() == 3);
    REQUIRE(a[1].size() == 3);
  }
}
