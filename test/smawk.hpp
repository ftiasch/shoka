#include "smawk.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>

struct Monge {
  using E = int;

  E operator()(int x, int y) const { return monge.at(x).at(y); }

  std::vector<std::vector<int>> monge;
};

TEST_CASE("smawk") {
  SMAWK<Monge> smawk;

  auto n = GENERATE(range(1, 50));
  auto m = GENERATE(range(1, 50));

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

  auto row_min = smawk(Monge{monge}, n, m);

  for (int i = 0; i < n; i++) {
    std::pair<int, int> best{INT_MAX, 0};
    for (int j = 0; j < m; j++) {
      best = std::min(best, {monge[i][j], j});
    }
    REQUIRE(row_min[i] == best.first);
  }
}
