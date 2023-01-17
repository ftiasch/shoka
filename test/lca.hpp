#include "lca.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>

TEST_CASE("LCA") {
  auto n = GENERATE(range(1, 100));

  std::minstd_rand gen{Catch::getSeed()};

  std::vector<int> parent(n), depth(n);
  std::vector<std::vector<int>> tree(n);
  parent[0] = -1;
  depth[0] = 0;
  for (int i = 1; i < n; ++i) {
    int p = parent[i] = std::uniform_int_distribution<>(0, i - 1)(gen);
    depth[i] = depth[p] + 1;
    tree[i].push_back(p);
    tree[p].push_back(i);
  }

  Lca lca(tree, 0);
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      int x = i, y = j;
      while (x != y) {
        auto &z = depth[x] > depth[y] ? x : y;
        z = parent[z];
      }
      REQUIRE(lca(i, j) == x);
    }
  }
}
