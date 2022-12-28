#include "binpow.h"
#include "fixed_size_matrix.h"
#include "mod.h"

#include "debug.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>

TEST_CASE("binpow") {
  SECTION("ModT") {
    REQUIRE(binpow(ModT<998'244'353>{233}, 998'244'352).get() == 1);
  }

  SECTION("FixedSizeMatrixT") {
    using Mod = ModT<7>;
    using Matrix = FixedSizeMatrixT<Mod, 2>;
    auto r = binpow(Matrix{{Mod{2}, Mod{3}}, {Mod{4}, Mod{5}}}, 2016);
    REQUIRE(r[0][0].get() == 1);
    REQUIRE(r[0][1].get() == 0);
    REQUIRE(r[1][0].get() == 0);
    REQUIRE(r[1][1].get() == 1);
  }
}
