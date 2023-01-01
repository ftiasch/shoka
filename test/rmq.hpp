#include "sparse_table.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>

TEST_CASE("sparse_table") {
  auto n = GENERATE(range(1, 100));
  std::minstd_rand gen{Catch::getSeed()};
  std::uniform_int_distribution<> dist(1, n);
  std::vector<int> a(n);
  for (int i = 0; i < n; ++i) {
    a[i] = dist(gen);
  }
  using Semilattice = SemilatticeT<int, INT_MAX>;
  SparseTable<Semilattice> rmq(a);
  for (int i = 0; i <= n; ++i) {
    REQUIRE(rmq(i, i) == Semilattice::id);
    Semilattice::T running_meet = Semilattice::id;
    for (int j = i; j < n; ++j) {
      running_meet = Semilattice::meet(running_meet, a[j]);
      REQUIRE(rmq(i, j + 1) == running_meet);
    }
  }
}
