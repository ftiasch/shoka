#include <catch2/catch_all.hpp>

#include <bits/stdc++.h>

#include "fast_io.h"

TEST_CASE("fast_io") {
  SECTION("input") {
    auto tmpf = std::tmpfile();
    std::fprintf(tmpf, "-2147483647 0 2147483647\n-9223372036854775807 0 "
                       "9223372036854775807\n");
    for (int i = 0; i < 100; ++i) {
      std::fprintf(tmpf, "%d\n", i);
    }
    std::rewind(tmpf);

    FastIO io{tmpf};

    REQUIRE(io.read() == -2147483647);
    REQUIRE(io.read() == 0);
    REQUIRE(io.read() == 2147483647);
    REQUIRE(io.read<long long>() == -9223372036854775807);
    REQUIRE(io.read<long long>() == 0);
    REQUIRE(io.read<long long>() == 9223372036854775807);

    REQUIRE(io.read_t<int, int>() == std::tuple<int, int>{0, 1});
    REQUIRE(io.read_v(3) == std::vector<int>{2, 3, 4});

    std::fclose(tmpf);
  }

  SECTION("output") {
    auto tmpf = std::tmpfile();
    FastIO io{nullptr, tmpf};
    io << INT_MIN << " " << INT_MAX << " " << LONG_MIN << " " << LONG_MAX
       << "\n";
    io << false << " " << true << "\n";
    io << std::vector<int>{2, 3, 3} << "\n";
  }
}
