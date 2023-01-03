#include "mod.h"
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

} // namespace rolling_hash

TEST_CASE("rolling_hash") {
  auto s = rolling_hash::fib(10);
  int n = s.size();
  using Mod = ModT<998'244'353>;
  using Hash = RollingHashT<Mod>;
  std::minstd_rand gen{Catch::getSeed()};
  Hash::initialize(gen);
  std::vector<Hash> prefix_hash(n + 1);
  for (int i = 0; i < n; ++i) {
    prefix_hash[i + 1] = prefix_hash[i] + Mod{s[i]};
  }
  for (int i = 0; i <= n; ++i) {
    REQUIRE(prefix_hash[i].get_len() == i);
  }
  for (int i = 0; i <= n; ++i) {
    REQUIRE((prefix_hash[i] - prefix_hash[i]) == Hash{});
    Hash range_hash;
    for (int j = i; j < n; ++j) {
      range_hash = range_hash + Mod{s[j]};
      REQUIRE((prefix_hash[j + 1] - prefix_hash[i]) == range_hash);
    }
  }
}
