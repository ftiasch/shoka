#include "../sparse_table.h"

#include "gtest/gtest.h"

#include <random>

TEST(SparseTable, Test) {
  const int n = 1000;
  using T = std::pair<uint32_t, int>;
  static SparseTable<T, n> st;
  std::mt19937 gen(0);
  std::vector<T> a(n);
  for (int i = 0; i < n; ++i) {
    a[i].first = gen();
    a[i].second = i;
  }
  st.compute(n, a.data());
  for (int i = 0; i < n; ++i) {
    T run_min = a[i];
    ASSERT_EQ(st.rmq(i, i + 1), run_min);
    for (int j = i + 1; j < n; ++j) {
      run_min = std::min(run_min, a[j]);
      ASSERT_EQ(st.rmq(i, j + 1), run_min);
    }
  }
}
