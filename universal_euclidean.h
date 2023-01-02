#include "binpow.h"

#include <algorithm>
#include <cstdint>
#include <type_traits>

template <typename Monoid> struct UniversalEuclidean {
  template <typename T> Monoid operator()(T n, T a, T b, T c) const {
    static_assert(std::is_integral_v<T>);
    auto r = Monoid::R();
    auto u = Monoid::U();
    auto prefix = u.power(b / c) * r;
    auto suffix = Monoid::mul_id();
    b %= c;
    while (true) {
      if (a >= c) {
        r = u.power(a / c) * r;
        a %= c;
      } else {
        auto m = (a * n + b) / c;
        if (m == 0) {
          return prefix * r.power(n) * suffix;
        }
        prefix = prefix * r.power((c - b - 1) / a) * u;
        suffix = r.power(n - (c * m - b - 1) / a) * suffix;
        b = (c - b - 1) % a;
        std::swap(a, c);
        n = m - 1;
        std::swap(u, r);
      }
    }
  }
};
