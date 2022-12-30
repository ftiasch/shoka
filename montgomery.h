#pragma once

#include "binpow.h"
#include "primality_test.h"

#include <cstdint>
#include <iostream>
#include <limits>

namespace montgomery_details {

template <typename M> static constexpr M mont_modinv(M MOD, int n) {
  M result = 1;
  for (int i = 0; i < n; ++i) {
    result *= 2 - MOD * result;
  }
  return -result;
}

template <typename M, M MOD> struct MultiplierT;

template <uint32_t MOD> struct MultiplierT<uint32_t, MOD> {
  using M = uint32_t;
  using M2 = uint64_t;
  static constexpr M M_MAX = UINT32_MAX;
  static constexpr int M_BITS = 32;
  static constexpr uint32_t INV = mont_modinv(MOD, 5);
};

template <uint64_t MOD> struct MultiplierT<uint64_t, MOD> {
  using M = uint64_t;
  using M2 = __uint128_t;
  static constexpr M M_MAX = UINT64_MAX;
  static constexpr int M_BITS = 64;
  static constexpr uint64_t INV = mont_modinv(MOD, 6);
};

template <typename M, M MOD_, bool PRIMALITY_CERTIFIED> struct MontgomeryBaseT {
  static const M MOD = MOD_;
  static_assert(MOD <= std::numeric_limits<M>::max() >> 2, "4 * MOD <= MAX");

  using Multiplier = MultiplierT<M, MOD>;
  using M2 = typename Multiplier::M2;

  template <typename T = M>
  explicit constexpr MontgomeryBaseT(T x_ = 0) : x{mont_multiply(x_, R2)} {}

  static constexpr MontgomeryBaseT mul_id() { return MontgomeryBaseT{1}; }

  static constexpr MontgomeryBaseT normalize(M2 x) {
    return MontgomeryBaseT{x % MOD};
  }

#ifdef SHOKA_TESTING
  static void set_mod(M) {}
#endif

  constexpr M get() const {
    auto y = reduce(x);
    return y >= MOD ? y - MOD : y;
  }

  constexpr MontgomeryBaseT &operator+=(const MontgomeryBaseT &other) {
    x += other.x;
    if (x >= MOD2) {
      x -= MOD2;
    }
    return *this;
  }

  constexpr MontgomeryBaseT operator+(const MontgomeryBaseT &other) const {
    MontgomeryBaseT copy = *this;
    return copy += other;
  }

  constexpr MontgomeryBaseT &operator-=(const MontgomeryBaseT &other) {
    x += MOD2 - other.x;
    if (x >= MOD2) {
      x -= MOD2;
    }
    return *this;
  }

  constexpr MontgomeryBaseT operator-() const {
    MontgomeryBaseT copy{0};
    return copy -= *this;
  }

  constexpr MontgomeryBaseT operator-(const MontgomeryBaseT &other) const {
    MontgomeryBaseT copy = *this;
    return copy -= other;
  }

  constexpr MontgomeryBaseT operator*=(const MontgomeryBaseT &other) {
    x = mont_multiply(x, other.x);
    return *this;
  }

  constexpr MontgomeryBaseT operator*(const MontgomeryBaseT &other) const {
    MontgomeryBaseT copy = *this;
    return copy *= other;
  }

  constexpr MontgomeryBaseT inv() const {
    static_assert(PRIMALITY_CERTIFIED || is_prime(MOD), "MOD is not a prime");
    return binpow(*this, MOD - 2);
  }

private:
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

  static constexpr M mont_multiply(M x, M y) {
    return reduce(static_cast<M2>(x) * static_cast<M2>(y));
  }

  static constexpr M reduce(M2 x) {
    M2 y =
        (((x & Multiplier::M_MAX) * Multiplier::INV) & Multiplier::M_MAX) * MOD;
    return (x + y) >> Multiplier::M_BITS;
  }

  static constexpr M MOD2 = MOD << 1;
  static constexpr M R = rpower(std::numeric_limits<M>::digits);
  static constexpr M R2 = rpower(std::numeric_limits<M2>::digits);

  M x;
};

} // namespace montgomery_details

namespace std {

template <typename M, M MOD, bool PRIMALITY_CERTIFIED>
ostream &operator<<(ostream &out, const montgomery_details::MontgomeryBaseT<
                                      M, MOD, PRIMALITY_CERTIFIED> &mod) {
  return out << mod.get();
}

} // namespace std

template <uint32_t MOD>
using MontgomeryT = montgomery_details::MontgomeryBaseT<uint32_t, MOD, false>;

template <uint64_t MOD>
using Montgomery64T = montgomery_details::MontgomeryBaseT<uint64_t, MOD, true>;
