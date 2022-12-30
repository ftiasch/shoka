#pragma once

#include "singleton.h"

#include <cstdint>
#include <iostream>
#include <limits>
#include <type_traits>

namespace barrett_details {

template <typename M> struct Multiplier;

template <> struct Multiplier<uint32_t> {
  using M2 = uint64_t;

  static M2 mul_hi(M2 x, M2 y) {
    return static_cast<__uint128_t>(x) * static_cast<__uint128_t>(y) >> 64;
  }
};

template <> struct Multiplier<uint64_t> {
  using M2 = __uint128_t;

  static M2 mul_hi(M2 x, M2 y) {
    M2 x_lo = x & UINT64_MAX;
    M2 x_hi = x >> 64;
    M2 y_lo = y & UINT64_MAX;
    M2 y_hi = y >> 64;
    M2 lo_lo = x_lo * y_lo;
    M2 hi_lo = x_hi * y_lo;
    M2 lo_hi = x_lo * y_hi;
    M2 cy = (hi_lo & UINT64_MAX) + (lo_hi & UINT64_MAX) + (lo_lo >> 64);
    return x_hi * y_hi + (hi_lo >> 64) + (lo_hi >> 64) + (cy >> 64);
  }
};

template <typename M, int PHANTOM> struct BarrettModBaseT {
  using M2 = typename Multiplier<M>::M2;

  template <typename T = M>
  explicit constexpr BarrettModBaseT(T x_ = 0) : x{static_cast<M>(x_)} {}

  static constexpr BarrettModBaseT mul_id() { return BarrettModBaseT{1}; }

  static void set_mod(M mod_) { store().set_mod(mod_); }

  template <typename T = M2>
  static constexpr std::enable_if_t<std::is_integral_v<T>, BarrettModBaseT>
  normalize(T x) {
    return BarrettModBaseT{reduce(x)};
  }

  static M mod() { return store().mod; }

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
  struct Store {
    void set_mod(M mod_) {
      mod = mod_;
      inv_mod = static_cast<M2>(-1) / mod - 1;
    }

    M reduce(M2 x) const {
      auto q = Multiplier<M>::mul_hi(x, inv_mod);
      auto r = x - q * mod;
      return r >= mod ? r - mod : r;
    }

    M mod;
    M2 inv_mod;
  };

  static Store &store() { return Singleton<Store>::instance(); }

  static M reduce(M2 x_) { return store().reduce(x_); }

  M x;
};

} // namespace barrett_details

namespace std {

template <typename M, int PHANTOM>
ostream &operator<<(ostream &out,
                    const barrett_details::BarrettModBaseT<M, PHANTOM> &m) {
  return out << m.get();
}

} // namespace std

template <int PHANTOM = 0>
using BarrettModT = barrett_details::BarrettModBaseT<uint32_t, PHANTOM>;
template <int PHANTOM = 0>
using BarrettMod64T = barrett_details::BarrettModBaseT<uint64_t, PHANTOM>;
