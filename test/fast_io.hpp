#include "fast_io.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>

TEST_CASE("fast_io") {
  auto tmpf = std::tmpfile();
  std::fprintf(tmpf, "%d 0 %d\n", std::numeric_limits<int>::min(),
               std::numeric_limits<int>::max());
  std::fprintf(tmpf, "%lld 0 %lld\n", std::numeric_limits<long long>::min(),
               std::numeric_limits<long long>::max());
  std::rewind(tmpf);
  FastIO io{tmpf};
  REQUIRE(io.read1() == std::numeric_limits<int>::min());
  REQUIRE(io.read1() == 0);
  REQUIRE(io.read1() == std::numeric_limits<int>::max());
  REQUIRE(io.read1<long long>() == std::numeric_limits<long long>::min());
  REQUIRE(io.read1<long long>() == 0);
  REQUIRE(io.read1<long long>() == std::numeric_limits<long long>::max());
  std::fclose(tmpf);
}
