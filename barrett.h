#pragma once

#include "singleton.h"

#include <cstdint>
#include <iostream>
#include <limits>
#include <type_traits>

namespace barrett_details {

template <typename M> struct Multiplier;

template <> struct Multiplier<uint64_t> {
  static uint64_t mul_hi(uint64_t x, uint64_t y) {
    return static_cast<__uint128_t>(x) * static_cast<__uint128_t>(y) >> 64;
  }
};

template <> struct Multiplier<__uint128_t> {
  static __uint128_t mul_hi(__uint128_t x, __uint128_t y) {
    __uint128_t a = x & UINT64_MAX, b = x >> 64, c = y & UINT64_MAX,
                d = y >> 64, ac = a * c, bc = b * c, ad = a * d,
                z = (bc & UINT64_MAX) + (ad & UINT64_MAX) + (ac >> 64);
    return b * d + (bc >> 64) + (ad >> 64) + (z >> 64);
  }
};

template <typename M, typename M2, typename PHANTOM> struct BarrettModBaseT {
  template <typename T = M>
  explicit constexpr BarrettModBaseT(T x_ = 0) : x{static_cast<M>(x_)} {}

  static constexpr BarrettModBaseT mul_id() { return BarrettModBaseT{1}; }

  static void set_mod(M mod_) { storage().set_mod(mod_); }

  template <typename T = M2>
  static constexpr std::enable_if_t<std::is_integral_v<T>, BarrettModBaseT>
  normalize(T x) {
    return BarrettModBaseT{reduce(x)};
  }

  constexpr M get() const { return x; }

  constexpr BarrettModBaseT &operator+=(const BarrettModBaseT &other) {
    x += other.x;
    if (x >= mod()) {
      x -= mod();
    }
    return *this;
  }

  constexpr BarrettModBaseT operator+(const BarrettModBaseT &other) const {
    BarrettModBaseT copy = *this;
    return copy += other;
  }

  constexpr BarrettModBaseT &operator-=(const BarrettModBaseT &other) {
    x += mod() - other.x;
    if (x >= mod()) {
      x -= mod();
    }
    return *this;
  }

  constexpr BarrettModBaseT operator-() const {
    BarrettModBaseT copy{0};
    copy -= *this;
    return copy;
  }

  constexpr BarrettModBaseT operator-(const BarrettModBaseT &other) const {
    BarrettModBaseT copy = *this;
    return copy -= other;
  }

  constexpr BarrettModBaseT operator*=(const BarrettModBaseT &other) {
    x = reduce(static_cast<M2>(x) * static_cast<M2>(other.x));
    return *this;
  }

  constexpr BarrettModBaseT operator*(const BarrettModBaseT &other) const {
    BarrettModBaseT copy = *this;
    return copy *= other;
  }

  constexpr BarrettModBaseT inv() const {
    return x == 1
               ? BarrettModBaseT{1}
               : -BarrettModBaseT{mod() / x} * BarrettModBaseT{mod() % x}.inv();
  }

private:
  struct Storage {
    void set_mod(M mod_) {
      mod = mod_;
      inv_mod = static_cast<M2>(-1) / mod - 1;
    }

    M reduce(M2 x) const {
      auto q = Multiplier<M2>::mul_hi(x, inv_mod);
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

} // namespace barrett_details

namespace std {

template <typename M, typename M2, typename PHANTOM>
ostream &operator<<(ostream &out,
                    const barrett_details::BarrettModBaseT<M, M2, PHANTOM> &m) {
  return out << m.get();
}

} // namespace std

template <typename PHANTOM = void>
using BarrettModT =
    barrett_details::BarrettModBaseT<uint32_t, uint64_t, PHANTOM>;
template <typename PHANTOM = void>
using BarrettMod64T =
    barrett_details::BarrettModBaseT<uint64_t, __uint128_t, PHANTOM>;
