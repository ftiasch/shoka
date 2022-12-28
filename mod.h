#pragma once

#include "primality_test.h"

#include <cstdint>
#include <iostream>
#include <limits>
#include <type_traits>

template <uint32_t MOD_> struct ModT {
  static constexpr uint32_t MOD = MOD_;

  static_assert(MOD <= (std::numeric_limits<uint32_t>::max() >> 1));

  template <typename T = uint32_t>
  explicit constexpr ModT(T x_ = 0) : x{static_cast<uint32_t>(x_)} {}

  static constexpr ModT mul_id() { return ModT{1}; }

  template <typename T = uint64_t>
  static constexpr std::enable_if_t<std::is_integral_v<T>, ModT>
  normalize(T x) {
    return ModT(x % MOD);
  }

  constexpr uint32_t get() const { return x; }

  constexpr ModT &operator+=(const ModT &other) {
    x += other.x;
    if (x >= MOD) {
      x -= MOD;
    }
    return *this;
  }

  constexpr ModT operator+(const ModT &other) const {
    ModT copy = *this;
    return copy += other;
  }

  constexpr ModT &operator-=(const ModT &other) {
    x += MOD - other.x;
    if (x >= MOD) {
      x -= MOD;
    }
    return *this;
  }

  constexpr ModT operator-() const {
    ModT copy{0};
    copy -= *this;
    return copy;
  }

  constexpr ModT operator-(const ModT &other) const {
    ModT copy = *this;
    return copy -= other;
  }

  constexpr ModT operator*=(const ModT &other) {
    x = static_cast<uint64_t>(x) * static_cast<uint64_t>(other.x) % MOD;
    return *this;
  }

  constexpr ModT operator*(const ModT &other) const {
    ModT copy = *this;
    return copy *= other;
  }

  constexpr ModT inv() const {
    static_assert(is_prime(MOD), "MOD is not a prime");
    return x == 1 ? ModT{1} : -ModT{MOD / x} * ModT{MOD % x}.inv();
  }

  uint32_t x;
};

namespace std {

template <uint32_t MOD>
ostream &operator<<(ostream &out, const ModT<MOD> &mod) {
  return out << mod.get();
}

} // namespace std
