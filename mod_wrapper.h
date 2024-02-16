#pragma once

#include <experimental/type_traits>

#include "binpow.h"
#include "singleton.h"

#include <cstdint>
#include <iostream>
#include <limits>
#include <utility>

namespace mod {

template <typename M> struct MultiplierT {};

template <> struct MultiplierT<uint32_t> {
  using M2 = uint64_t;

  static constexpr int LOG_M_BITS = 5;

  static constexpr M2 mul_hi(M2 x, M2 y) {
    return static_cast<__uint128_t>(x) * static_cast<__uint128_t>(y) >> 64;
  }
};

template <> struct MultiplierT<uint64_t> {
  using M2 = __uint128_t;

  static constexpr int LOG_M_BITS = 6;

  static constexpr M2 mul_hi(M2 x, M2 y) {
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

template <typename M> using m2_t = typename MultiplierT<M>::M2;

template <typename Mod> struct ModWrapperT {
  using M = typename Mod::M;
  using M2 = m2_t<M>;

private:
  // traits
  template <typename T> using has_static_mod_t = decltype(T::MOD);
  static constexpr bool has_static_mod =
      std::experimental::is_detected_v<has_static_mod_t, Mod>;

  // Montgomery performs arith in the Montgomery domain
  template <typename T> using has_wrap_t = decltype(T::wrap(std::declval<M>()));
  static constexpr bool has_wrap =
      std::experimental::is_detected_v<has_wrap_t, Mod>;

public:
  static void set_mod(M mod) {
    if constexpr (!has_static_mod) {
      singleton<Mod>().set_mod(mod);
    }
  }

  static constexpr M mod() {
    if constexpr (has_static_mod) {
      return Mod::MOD;
    } else {
      return singleton<Mod>().get_mod();
    }
  }

  static constexpr ModWrapperT mul_id() { return ModWrapperT::normalize(1); }

  static constexpr ModWrapperT neg_id(uint64_t n) {
    return (n & 1) ? -ModWrapperT{1} : ModWrapperT{1};
  }

  static constexpr ModWrapperT normalize(M2 x) {
    return ModWrapperT{static_cast<M>(x % mod())};
  }

  constexpr ModWrapperT() : x{construct(0)} {}

  template <typename T = M>
  explicit constexpr ModWrapperT(T x_ = 0) : x{construct(static_cast<M>(x_))} {
    static_assert(std::numeric_limits<T>::digits <=
                  std::numeric_limits<M>::digits);
  }

  constexpr M get() const {
    if constexpr (has_wrap) {
      return Mod::unwrap(x);
    } else {
      return x;
    }
  }

  constexpr bool operator==(const ModWrapperT &other) const {
    return get() == other.get();
  }

  constexpr ModWrapperT &operator+=(const ModWrapperT &other) {
    if constexpr (has_wrap) {
      Mod::add(x, other.x);
    } else {
      x += other.x;
      if (x >= mod()) {
        x -= mod();
      }
    }
    return *this;
  }

  constexpr ModWrapperT &operator-=(const ModWrapperT &other) {
    if constexpr (has_wrap) {
      Mod::sub(x, other.x);
    } else {
      x += mod() - other.x;
      if (x >= mod()) {
        x -= mod();
      }
    }
    return *this;
  }

  constexpr ModWrapperT operator*=(const ModWrapperT &other) {
    auto p = static_cast<M2>(x) * static_cast<M2>(other.x);
    if constexpr (has_static_mod) {
      x = Mod::reduce(p);
    } else {
      x = singleton<Mod>().reduce(p);
    }
    return *this;
  }

  constexpr ModWrapperT operator/=(const ModWrapperT &other) {
    return *this *= other.inv();
  }

  constexpr ModWrapperT inv() const { return binpow(*this, mod() - 2); }

  // helper arith

  constexpr bool operator!=(const ModWrapperT &other) const {
    return !(*this == other);
  }

  constexpr ModWrapperT operator+(const ModWrapperT &other) const {
    ModWrapperT copy = *this;
    return copy += other;
  }

  constexpr ModWrapperT operator-() const {
    ModWrapperT copy{0};
    copy -= *this;
    return copy;
  }

  constexpr ModWrapperT operator-(const ModWrapperT &other) const {
    ModWrapperT copy = *this;
    return copy -= other;
  }

  constexpr ModWrapperT operator*(const ModWrapperT &other) const {
    ModWrapperT copy = *this;
    return copy *= other;
  }

  constexpr ModWrapperT operator/(const ModWrapperT &other) const {
    ModWrapperT copy = *this;
    return copy /= other;
  }

private:
  static constexpr M construct(M x) {
    if constexpr (has_wrap) {
      return Mod::wrap(x);
    } else {
      return x;
    }
  }

  M x;
};

} // namespace mod

namespace std {

template <typename Mod>
ostream &operator<<(ostream &out, const mod::ModWrapperT<Mod> &w) {
  return out << w.get();
}

} // namespace std
