#include "nd_array.h"
#include "nd_vector.h"

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

TEST_CASE("nd_vector") {
  SECTION("NDVector<int, 3>") {
    NDVector<int, 3> v{1, 2, 3};
    REQUIRE(v.size() == 1);
    REQUIRE(v[0].size() == 2);
    REQUIRE(v[0][0].size() == 3);
    REQUIRE(v[0][1].size() == 3);
  }

  SECTION("NDVector<int, 2> initialized with 233") {
    NDVector<int, 2> v{2, 3, 233};
    for (int i = 0; i < 2; ++i) {
      for (int j = 0; j < 3; ++j) {
        REQUIRE(v[i][j] == 233);
      }
    }
  }
}
