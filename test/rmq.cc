#include "../bit_rmq.h"
#include "../sparse_table.h"

#include "gtest/gtest.h"

#include <random>

template <template <typename, int> class RMQ> void test_rmq() {
  const int N = 100;
  using T = std::pair<uint32_t, int>;
  static RMQ<T, N> st;
  std::mt19937 gen(0);
  std::vector<T> a(N);
  for (int n = 1; n <= N; ++n) {
    for (int i = 0; i < n; ++i) {
      a[i].first = gen();
      a[i].second = i;
    }
    st.compute(n, a.data());
    for (int i = 0; i < n; ++i) {
      T run_min = a[i];
      ASSERT_EQ(st.rmq(i, i), run_min) << "n=" << n << "|l=" << i << "|r=" << i;
      for (int j = i + 1; j < n; ++j) {
        run_min = std::min(run_min, a[j]);
        ASSERT_EQ(st.rmq(i, j), run_min)
            << "n=" << n << "|l=" << i << "|r=" << j;
      }
    }
  }
}

TEST(SparseTable, Test) { test_rmq<SparseTable>(); }
TEST(BitRMQ, Test) { test_rmq<BitRMQ>(); }
