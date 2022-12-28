#include "mod.h"
#include "binpow.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>

TEST_CASE("ModT") {
    using Mod = ModT<998'244'353>;
    
    SECTION("constructor") {
        REQUIRE(Mod{233}.get() == 233);
        int x = 233;
        REQUIRE(Mod{x}.get() == 233);
        // (998'244'353 << 8) + 233
        REQUIRE(Mod::normalize(255550554601).get() == 233);
    }

    SECTION("inverse") {
        REQUIRE(binpow(Mod{233}, 998'244'352).get() == 1);
    }

    SECTION("inverse") {
        Mod x{233};
        REQUIRE((x * x.inv()).get() == 1);
    }
}
