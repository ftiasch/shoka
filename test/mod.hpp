#include "barrett.h"
#include "binpow.h"
#include "mod.h"
#include "montgomery.h"
#include "non_const_mod.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>

constexpr uint32_t MOD_32 = 998'244'353;
// https://primes.utm.edu/lists/2small/0bit.html
constexpr uint64_t MOD_64 = (1ULL << 62) - 57;

TEMPLATE_TEST_CASE("mod_32", "[template]", ModT<MOD_32>, MontgomeryT<MOD_32>,
                   BarrettModT<>, NonConstModT<>) {
  using Mod = TestType;

  Mod::set_mod(MOD_32);

  REQUIRE(Mod{233}.get() == 233);
  {
    int x = 233;
    REQUIRE(Mod{x}.get() == 233);
  }
  // (998'244'353 << 8) + 233
  REQUIRE(Mod::normalize(255550554601).get() == 233);

  REQUIRE(binpow(Mod{233}, MOD_32 - 1).get() == 1);

  {
    Mod x{233};
    REQUIRE((x * x.inv()).get() == 1);
  }
}

TEMPLATE_TEST_CASE("mod_64", "[template]", Mod64T<MOD_64>,
                   Montgomery64T<MOD_64>, BarrettMod64T<>, NonConstMod64T<>) {
  using Mod = TestType;
  Mod::set_mod(MOD_64);

  REQUIRE(Mod{233}.get() == 233);
  {
    int x = 233;
    REQUIRE(Mod{x}.get() == 233);
  }

  REQUIRE(binpow(Mod{233}, MOD_64 - 1).get() == 1);

  {
    Mod x{233};
    REQUIRE((x * x.inv()).get() == 1);
  }
}
