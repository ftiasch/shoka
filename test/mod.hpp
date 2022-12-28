#include "binpow.h"
#include "mod.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>

TEST_CASE("mod") {
  SECTION("ModT") {
    using Mod = ModT<998'244'353>;

    REQUIRE(Mod{233}.get() == 233);
    {
      int x = 233;
      REQUIRE(Mod{x}.get() == 233);
    }
    // (998'244'353 << 8) + 233
    REQUIRE(Mod::normalize(255550554601).get() == 233);

    REQUIRE(binpow(Mod{233}, 998'244'352).get() == 1);

    {
      Mod x{233};
      REQUIRE((x * x.inv()).get() == 1);
    }
  }

  SECTION("Mod64T") {
    // https://primes.utm.edu/lists/2small/0bit.html
    static constexpr auto MOD = (1ULL << 63) - 25;

    using Mod = Mod64T<MOD>;

    REQUIRE(Mod{233}.get() == 233);
    {
      int x = 233;
      REQUIRE(Mod{x}.get() == 233);
    }

    REQUIRE(binpow(Mod{233}, MOD - 1).get() == 1);

    {
      Mod x{233};
      REQUIRE((x * x.inv()).get() == 1);
    }
  }
}
