#include "binpow.h"
#include "montgomery.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>

TEST_CASE("montgomery") {
  SECTION("MontgomeryT") {
    using Mod = MontgomeryT<998'244'353>;

    REQUIRE(Mod{233}.get() == 233);
    {
      int x = 233;
      REQUIRE(Mod{x}.get() == 233);
    }

    REQUIRE(binpow(Mod{233}, 998'244'352).get() == 1);

    {
      Mod x{233};
      REQUIRE((x * x.inv()).get() == 1);
    }
  }

  SECTION("Montgomery64T") {
    // https://primes.utm.edu/lists/2small/0bit.html
    static constexpr auto MOD = (1ULL << 62) - 57;

    using Mod = Montgomery64T<MOD>;

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
