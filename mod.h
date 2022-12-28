#pragma once

#include "primality_test.h"

#include <cstdint>
#include <iostream>
#include <limits>
#include <type_traits>

namespace mod_details {

template <typename M, typename M2, M MOD_, bool PRIMALITY_CERTIFIED>
struct ModBaseT {
  static constexpr M MOD = MOD_;

  static_assert(MOD <= (std::numeric_limits<M>::max() >> 1));

  template <typename T = M>
  explicit constexpr ModBaseT(T x_ = 0) : x{static_cast<M>(x_)} {}

  static constexpr ModBaseT mul_id() { return ModBaseT{1}; }

  template <typename T = M2>
  static constexpr std::enable_if_t<std::is_integral_v<T>, ModBaseT>
  normalize(T x) {
    return ModBaseT(x % MOD);
  }

  constexpr M get() const { return x; }

  constexpr ModBaseT &operator+=(const ModBaseT &other) {
    x += other.x;
    if (x >= MOD) {
      x -= MOD;
    }
    return *this;
  }

  constexpr ModBaseT operator+(const ModBaseT &other) const {
    ModBaseT copy = *this;
    return copy += other;
  }

  constexpr ModBaseT &operator-=(const ModBaseT &other) {
    x += MOD - other.x;
    if (x >= MOD) {
      x -= MOD;
    }
    return *this;
  }

  constexpr ModBaseT operator-() const {
    ModBaseT copy{0};
    copy -= *this;
    return copy;
  }

  constexpr ModBaseT operator-(const ModBaseT &other) const {
    ModBaseT copy = *this;
    return copy -= other;
  }

  constexpr ModBaseT operator*=(const ModBaseT &other) {
    x = static_cast<M2>(x) * static_cast<M2>(other.x) % MOD;
    return *this;
  }

  constexpr ModBaseT operator*(const ModBaseT &other) const {
    ModBaseT copy = *this;
    return copy *= other;
  }

  constexpr ModBaseT inv() const {
    static_assert(PRIMALITY_CERTIFIED || is_prime(MOD), "MOD is not a prime");
    return x == 1 ? ModBaseT{1} : -ModBaseT{MOD / x} * ModBaseT{MOD % x}.inv();
  }

  M x;
};

} // namespace mod_details

namespace std {

template <typename M, typename M2, M MOD, bool PRIMALITY_CERTIFIED>
ostream &
operator<<(ostream &out,
           const mod_details::ModBaseT<M, M2, MOD, PRIMALITY_CERTIFIED> &m) {
  return out << m.get();
}

} // namespace std

template <uint32_t MOD>
using ModT = mod_details::ModBaseT<uint32_t, uint64_t, MOD, false>;
template <uint64_t MOD>
using Mod64T = mod_details::ModBaseT<uint64_t, __uint128_t, MOD, true>;
