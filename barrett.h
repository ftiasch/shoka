#pragma once

#include "singleton.h"

#include <cstdint>
#include <iostream>
#include <limits>
#include <type_traits>

namespace barret_details {

template <typename M>
struct Multiplier;

template <>
struct Multiplier<uint64_t> {
  static uint64_t multiply(uint64_t x, uint64_t y) {
    return static_cast<__uint128_t>(x) * static_cast<__uint128_t>(y) >> 64;
  }
};

template <>
struct Multiplier<__uint128_t> {
  static __uint128_t multiply(__uint128_t x, __uint128_t y) {
    __uint128_t x_hi = x >> 64;
    __uint128_t x_lo = x & UINT64_MAX;
    __uint128_t y_hi = y >> 64;
    __uint128_t y_lo = y & UINT64_MAX;
    return x_hi * y_hi + (x_hi * y_lo >> 64) + (x_lo * y_hi >> 64);
  }
};

template <typename M, typename M2, int PHANTOM> struct BarretModBaseT {
  template <typename T = M>
  explicit constexpr BarretModBaseT(T x_ = 0) : x{static_cast<M>(x_)} {}

  static constexpr BarretModBaseT mul_id() { return BarretModBaseT{1}; }

  static void set_mod(M mod_) { storage().set_mod(mod_); }

  template <typename T = M2>
  static constexpr std::enable_if_t<std::is_integral_v<T>, BarretModBaseT>
  normalize(T x) {
    return BarretModBaseT{reduce(x)};
  }

  constexpr M get() const { return x; }

  constexpr BarretModBaseT &operator+=(const BarretModBaseT &other) {
    x += other.x;
    if (x >= mod()) {
      x -= mod();
    }
    return *this;
  }

  constexpr BarretModBaseT operator+(const BarretModBaseT &other) const {
    BarretModBaseT copy = *this;
    return copy += other;
  }

  constexpr BarretModBaseT &operator-=(const BarretModBaseT &other) {
    x += mod() - other.x;
    if (x >= mod()) {
      x -= mod();
    }
    return *this;
  }

  constexpr BarretModBaseT operator-() const {
    BarretModBaseT copy{0};
    copy -= *this;
    return copy;
  }

  constexpr BarretModBaseT operator-(const BarretModBaseT &other) const {
    BarretModBaseT copy = *this;
    return copy -= other;
  }

  constexpr BarretModBaseT operator*=(const BarretModBaseT &other) {
    x = reduce(static_cast<M2>(x) * static_cast<M2>(other.x));
    return *this;
  }

  constexpr BarretModBaseT operator*(const BarretModBaseT &other) const {
    BarretModBaseT copy = *this;
    return copy *= other;
  }

private:
  struct Storage {
    void set_mod(M mod_) {
      mod = mod_;
      inv_mod = static_cast<M2>(-1) / mod + 1;
    }

    M reduce(M2 x) const {
      auto q = Multiplier<M2>::multiply(x, inv_mod);
      auto r = x - q * mod;
      return r >= mod ? r - mod : r;
    }

    M mod;
    M2 inv_mod;
  };

  static Storage &storage() { return Singleton<Storage>::instance(); }

  static M mod() { return storage().mod; }

  static M reduce(M2 x_) { return storage().reduce(x_); }

  M x;
};

} // namespace barret_details

namespace std {

template <typename M, typename M2, int PHANTOM>
ostream &operator<<(ostream &out,
                    const barret_details::BarretModBaseT<M, M2, PHANTOM> &m) {
  return out << m.get();
}

} // namespace std

template <int PHANTOM = 0>
using BarretModT = barret_details::BarretModBaseT<uint32_t, uint64_t, PHANTOM>;
template <int PHANTOM = 0>
using BarretMod64T =
    barret_details::BarretModBaseT<uint64_t, __uint128_t, PHANTOM>;
