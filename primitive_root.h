#pragma once

#include "binpow.h"

template <typename Mod> struct FiniteField {
  static constexpr Mod primitive_root() {
    int g = 2;
    while (!is_primitive_root(Mod{g})) {
      g++;
    }
    return Mod{g};
  }

private:
  static constexpr bool is_primitive_root(Mod g) {
    for (int d = 2; d * d <= Mod::mod() - 1; ++d) {
      if ((Mod::mod() - 1) % d == 0 &&
          (binpow(g, d).get() == 1 ||
           binpow(g, (Mod::mod() - 1) / d).get() == 1)) {
        return false;
      }
    }
    return true;
  }
};
