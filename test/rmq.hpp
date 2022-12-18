#include "sparse_table.h"
#include "bit_rmq.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>

namespace rmq {

template <template <typename, typename> class RMQ, typename Compare>
void test_helper() {
  constexpr int n = 5;

  using namespace Catch::Generators;
  auto A = GENERATE(take(1, chunk(n, random(0U, ~0U))));

  using T = std::pair<uint32_t, int>;
  std::vector<T> a(n);
  for (int i = 0; i < n; ++i) {
    a[i].first = A[i];
    a[i].second = i;
  }

  RMQ<T, Compare> rmq(a);
  Compare compare;
  for (int i = 0; i < n; ++i) {
    T run_min = a[i];
    REQUIRE(rmq(i, i) == run_min);
    for (int j = i + 1; j < n; ++j) {
      run_min = compare(run_min, a[j]) ? run_min : a[j];
      REQUIRE(rmq(i, j) == run_min);
    }
  }
}

} // namespace rmq

TEST_CASE("rmq") {
  SECTION("SparseTable<T, std::less>") {
    rmq::test_helper<SparseTable, std::less<>>();
  }

  SECTION("SparseTable<T, std::greater>") {
    rmq::test_helper<SparseTable, std::greater<>>();
  }
}
