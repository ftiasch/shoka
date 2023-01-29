#pragma once

#include "mod_wrapper.h"

namespace mod {

template <typename M_, M_ MOD_> struct MontBaseT {
  using M = M_;
  static constexpr M MOD = MOD_;

  static_assert(MOD <= (std::numeric_limits<M_>::max() >> 2));

  static constexpr M wrap(M x) {
    return reduce(static_cast<M2>(x) * static_cast<M2>(R2));
  }

  static constexpr M unwrap(M x) {
    M reduced_x = reduce(x);
    return reduced_x >= MOD ? reduced_x - MOD : reduced_x;
  }

  static constexpr void add(M &x, M y) {
    x += y;
    if (x >= MOD2) {
      x -= MOD2;
    }
  }

  static constexpr void sub(M &x, M y) {
    x += MOD2 - y;
    if (x >= MOD2) {
      x -= MOD2;
    }
  }

private:
  using M2 = m2_t<M>;

public:
  static constexpr M reduce(M2 x) {
    M2 y = (((x & M_MASK) * INV) & M_MASK) * MOD;
    return (x + y) >> M_BITS;
  }

private:
  static constexpr auto M_BITS = std::numeric_limits<M>::digits;
  static constexpr auto M_MASK = std::numeric_limits<M>::max();

  static constexpr M MOD2 = MOD << 1;

  static constexpr M rpower(int n) {
    M result = 1;
    for (int i = 0; i < n; ++i) {
      result += result;
      if (result >= MOD) {
        result -= MOD;
      }
    }
    return result;
  }

  static constexpr M R = rpower(M_BITS);
  static constexpr M R2 = rpower(M_BITS << 1);

  static constexpr M mont_modinv(int n) {
    M result = 1;
    for (int i = 0; i < n; ++i) {
      result *= 2 - MOD * result;
    }
    return -result;
  }

  static constexpr M INV = mont_modinv(MultiplierT<M>::LOG_M_BITS);
};

template <uint64_t M> using Mont64T = ModWrapperT<MontBaseT<uint64_t, M>>;
template <uint32_t M> using MontT = ModWrapperT<MontBaseT<uint32_t, M>>;

} // namespace mod

using mod::Mont64T;
using mod::MontT;
