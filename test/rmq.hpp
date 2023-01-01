#include "bit_rmq.h"
#include "sparse_table.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>

namespace rmq {

template <typename T, T id, typename Compare>
struct SparseTableAdapterT : public SparseTableT<SemilatticeT<T, id, Compare>> {
  using Base = SparseTableT<SemilatticeT<T, id, Compare>>;

  using Base::Semilattice;
  using Base::SparseTableT;
  using Base::operator();
};

template <typename RmqT>
static void test_helper(const std::vector<typename RmqT::T> &values) {
  int n = values.size();
  RmqT rmq{std::vector(values)};
  using Semilattice = typename RmqT::Semilattice;
  for (int i = 0; i <= n; ++i) {
    REQUIRE(rmq(i, i) == Semilattice::id);
    auto running_meet = Semilattice::id;
    for (int j = i; j < n; ++j) {
      running_meet = Semilattice::meet(running_meet, values[j]);
      REQUIRE(rmq(i, j + 1) == running_meet);
    }
  }
}

struct TestSemilattice {
  using T = std::pair<int, int>;

  static constexpr T id{INT_MAX, INT_MIN};

  static constexpr T meet(const T &x, const T &y) {
    return T{
        std::min(x.first, y.first),
        std::max(x.second, y.second),
    };
  }
};

} // namespace rmq

TEMPLATE_PRODUCT_TEST_CASE_SIG("rmq", "[template][product][nttp]",
                               ((typename T, T id, typename Compare), T, id,
                                Compare),
                               (rmq::SparseTableAdapterT, BitRmqT),
                               ((int, INT_MAX, std::less<int>),
                                (int, 0, std::greater<int>))) {
  auto n = GENERATE(range(1, 100));
  std::minstd_rand gen{Catch::getSeed()};
  std::uniform_int_distribution<> dist(1, n);
  std::vector<int> a(n);
  for (int i = 0; i < n; ++i) {
    a[i] = dist(gen);
  }
  rmq::test_helper<TestType>(a);
}

TEST_CASE("rmq_strange") {
  auto n = GENERATE(range(1, 100));
  std::minstd_rand gen{Catch::getSeed()};
  std::uniform_int_distribution<> dist(1, n);
  using T = std::pair<int, int>;
  std::vector<T> a(n);
  for (int i = 0; i < n; ++i) {
    a[i].first = dist(gen);
    a[i].second = dist(gen);
  }
  rmq::test_helper<SparseTableT<rmq::TestSemilattice>>(a);
}
