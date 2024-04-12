#include "smawk.h"

#include "debug.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>

struct MongeCompare {
  int operator()(int r, int c0, int c1) const {
    return monge[r][c0] - monge[r][c1];
  }

  std::vector<std::vector<int>> monge;
};

TEST_CASE("smawk") {
  auto n = GENERATE(range(1, 100));
  auto m = GENERATE(range(1, 100));

  std::mt19937 gen{Catch::getSeed()};
  std::vector monge(n, std::vector<int>(m));
  for (int i = 0; i < n; i++) {
    for (int j = m; j--;) {
      monge[i][j] = gen() & 1;
      if (i) {
        monge[i][j] += monge[i - 1][j];
      }
      if (j + 1 < m) {
        monge[i][j] += monge[i][j + 1];
      }
      if (i && j + 1 < m) {
        monge[i][j] -= monge[i - 1][j + 1];
      }
    }
  }

  auto row_min = smawk(n, m, MongeCompare{monge});

  for (int i = 0; i < n; i++) {
    std::pair<int, int> best{INT_MAX, 0};
    for (int j = 0; j < m; j++) {
      best = std::min(best, {monge[i][j], j});
    }
    REQUIRE(row_min[i] == best.second);
  }
}
