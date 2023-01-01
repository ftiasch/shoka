#pragma once

#include <experimental/type_traits>

#include "binpow.h"
#include "singleton.h"

#include <cstdint>
#include <iostream>
#include <limits>
#include <utility>

namespace v2 {

namespace mod_details {

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
  template <typename T>
  using has_set_mod_t =
      decltype(std::declval<T &>().set_mod(std::declval<M>()));
  template <typename T> using has_static_mod_t = decltype(T::MOD);
  static constexpr bool has_static_mod =
      std::experimental::is_detected_v<has_static_mod_t, Mod>;

  // Montgomery performs arith in the Montgomery domain
  template <typename T> using has_wrap_t = decltype(T::wrap(std::declval<M>()));
  static constexpr bool has_wrap =
      std::experimental::is_detected_v<has_wrap_t, Mod>;

public:
  static void set_mod(M mod) {
    if constexpr (std::experimental::is_detected_v<has_set_mod_t, Mod>) {
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

  static constexpr ModWrapperT normalize(M2 x) {
    return ModWrapperT{static_cast<M>(x % mod())};
  }

  template <typename T = M> explicit constexpr ModWrapperT(T x_ = 0) {
    static_assert(std::numeric_limits<T>::digits <=
                  std::numeric_limits<M>::digits);
    if constexpr (has_wrap) {
      x = Mod::wrap(static_cast<M>(x_));
    } else {
      x = static_cast<M>(x_);
    }
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

  constexpr ModWrapperT inv() const {
    if constexpr (has_wrap) {
      return binpow(*this, Mod::mod() - 2);
    } else {
      return x == 1 ? ModWrapperT{1}
                    : -ModWrapperT{mod() / x} * ModWrapperT{mod() % x}.inv();
    }
  }

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

private:
  M x;
};

template <typename M_, M_ MOD_> struct ModBaseT {
  using M = M_;
  static constexpr M MOD = MOD_;

  static_assert((MOD - 1) <= (std::numeric_limits<M_>::max() >> 1));

private:
  using M2 = m2_t<M>;

public:
  static constexpr M reduce(M2 x) { return x % MOD; }
};

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

template <typename M_> struct BarrettBaseT {
  using M = M_;

  void set_mod(M mod_) {
    mod = mod_;
    inv_mod = static_cast<M2>(-1) / mod;
  }

  M get_mod() const { return mod; }

private:
  using M2 = m2_t<M>;

public:
  M reduce(M2 x) {
    auto q = MultiplierT<M>::mul_hi(x, inv_mod);
    auto r = x - q * mod;
    return r >= mod ? r - mod : r;
  }

private:
  M mod;
  M2 inv_mod;
};

using Barrett64T = ModWrapperT<BarrettBaseT<uint64_t>>;
using BarrettT = ModWrapperT<BarrettBaseT<uint32_t>>;
template <uint64_t M> using Mod64T = ModWrapperT<ModBaseT<uint64_t, M>>;
template <uint32_t M> using ModT = ModWrapperT<ModBaseT<uint32_t, M>>;
template <uint64_t M> using Mont64T = ModWrapperT<MontBaseT<uint64_t, M>>;
template <uint32_t M> using MontT = ModWrapperT<MontBaseT<uint32_t, M>>;

} // namespace mod_details

using mod_details::Barrett64T;
using mod_details::BarrettT;
using mod_details::Mod64T;
using mod_details::ModT;
using mod_details::Mont64T;
using mod_details::MontT;

} // namespace v2

namespace std {

template <typename Mod>
ostream &operator<<(ostream &out, const v2::mod_details::ModWrapperT<Mod> &w) {
  return out << w.get();
}

} // namespace std
