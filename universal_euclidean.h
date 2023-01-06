#include "binpow.h"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>

template <typename Monoid> struct UniversalEuclidean {
  // sum_{i = 0}^{n} (a * i + b) / c
  Monoid operator()(uint64_t n, uint64_t a, uint64_t b, uint64_t c) const {
    if (n && a > (std::numeric_limits<uint64_t>::max() - b) / n) {
      throw std::invalid_argument("a * n + b >= 2^64");
    }
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
