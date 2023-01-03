#include "mod.h"
#include "mont.h"
#include "rolling_hash.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>

namespace rolling_hash {

std::string fib(int n) {
  std::string a = "a", b = "b";
  for (int i = 1; i < n; ++i) {
    std::tie(a, b) = std::make_pair(b, a + b);
  }
  return a;
}

using Mod1 = ModT<998'244'353>;
using Mod2 = ModT<1'000'000'007>;

using SingleHash = RollingHashT<Mod1>;
using DoubleHash1 = RollingHashT<Mod1, Mod2>;
using DoubleHash2 = RollingHashT<Mod1, MontT<1'000'000'007>>;

} // namespace rolling_hash

TEMPLATE_TEST_CASE("rolling_hash", "[template]", rolling_hash::SingleHash,
                   rolling_hash::DoubleHash1, rolling_hash::DoubleHash2) {
  using Hash = TestType;

  std::minstd_rand gen{Catch::getSeed()};
  Hash::initialize(gen);

  auto s = rolling_hash::fib(10);
  int n = s.size();
  std::vector<Hash> prefix_hash(n + 1);
  for (int i = 0; i < n; ++i) {
    prefix_hash[i + 1] = prefix_hash[i] + Hash{s[i]};
  }
  for (int i = 0; i <= n; ++i) {
    REQUIRE(prefix_hash[i].length == i);
  }
  for (int i = 0; i <= n; ++i) {
    REQUIRE((prefix_hash[i] - prefix_hash[i]) == Hash{});
    Hash range_hash;
    for (int j = i; j < n; ++j) {
      range_hash = range_hash + Hash{s[j]};
      REQUIRE((prefix_hash[j + 1] - prefix_hash[i]) == range_hash);
    }
  }
  for (int i = 0; i <= n; ++i) {
    for (int j = i; j <= n; ++j) {
      for (int ii = 0; ii <= n; ++ii) {
        for (int jj = ii; jj <= n; ++jj) {
          bool hash_eq = (prefix_hash[j] - prefix_hash[i]) ==
                         (prefix_hash[jj] - prefix_hash[ii]);
          bool str_eq = s.substr(i, j - i) == s.substr(ii, jj - ii);
          REQUIRE(hash_eq == str_eq);
        }
      }
    }
  }
}
