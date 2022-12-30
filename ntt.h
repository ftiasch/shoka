#pragma once

#include "binpow.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <stdexcept>

template <typename Mod_> struct NttT {
  using Mod = Mod_;

  static void assert_power_of_two(int n) {
    if (n & (n - 1)) {
      throw std::invalid_argument(std::to_string(n) + " is not a power of two");
    }
  }

  static int min_power_of_two(int n) {
    return n == 1 ? 1 : 1 << (32 - __builtin_clz(n - 1));
  }

  static void dit(int n, Mod *a) {
    assert_power_of_two(n);
    for (int m = 1; m < n; m <<= 1) {
      const Mod root = binpow(G, (Mod::MOD - 1) / (m << 1));
      for (int i = 0; i < n; i += m << 1) {
        Mod twiddle(1);
        for (int r = i; r < i + m; ++r) {
          Mod tmp = twiddle * a[r + m];
          a[r + m] = a[r];
          a[r + m] -= tmp;
          a[r] += tmp;
          twiddle *= root;
        }
      }
    }
  }

  static void dif(int n, Mod *a) {
    assert_power_of_two(n);
    for (int m = n; m >>= 1;) {
      const Mod root = binpow(G, Mod::MOD - 1 - (Mod::MOD - 1) / (m << 1));
      for (int i = 0; i < n; i += m << 1) {
        Mod twiddle(1);
        for (int r = i; r < i + m; ++r) {
          Mod tmp = a[r];
          tmp -= a[r + m];
          a[r] += a[r + m];
          a[r + m] = twiddle * tmp;
          twiddle *= root;
        }
      }
    }
  }

  static Mod get_primitive_root() { return G; }

private:
  struct FiniteField {
    static constexpr Mod primitive_root() {
      int g = 2;
      while (!is_primitive_root(Mod{g})) {
        g++;
      }
      return Mod{g};
    }

  private:
    static constexpr bool is_primitive_root(Mod g) {
      for (int d = 2; d * d <= Mod::MOD - 1; ++d) {
        if ((Mod::MOD - 1) % d == 0 &&
            (binpow(g, d).get() == 1 ||
             binpow(g, (Mod::MOD - 1) / d).get() == 1)) {
          return false;
        }
      }
      return true;
    }
  };

  static constexpr Mod G = FiniteField::primitive_root();
};
