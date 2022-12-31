#include "fast_io.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>

TEST_CASE("fast_io") {
  auto tmpf = std::tmpfile();
  auto big_int = INT_MAX;
  long long big_int64 = INT64_MAX;
  std::fprintf(tmpf, "%d 0 %d\n", -big_int, big_int);
  std::fprintf(tmpf, "%lld 0 %lld\n", -big_int64, big_int64);
  std::rewind(tmpf);
  FastIO io{tmpf};
  REQUIRE(io.read1<>() == -big_int);
  REQUIRE(io.read1<>() == 0);
  REQUIRE(io.read1<>() == big_int);
  REQUIRE(io.read1<long long>() == -big_int64);
  REQUIRE(io.read1<long long>() == 0);
  REQUIRE(io.read1<long long>() == big_int64);
  std::fclose(tmpf);
}
