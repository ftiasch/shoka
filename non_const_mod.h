#pragma once

#include "singleton.h"

#include <cstdint>
#include <iostream>
#include <limits>
#include <type_traits>

namespace non_const_mod_details {

template <typename M> struct Multiplier;

template <> struct Multiplier<uint32_t> { using M2 = uint64_t; };

template <> struct Multiplier<uint64_t> { using M2 = __uint128_t; };

template <typename M, int PHANTOM> struct NonConstModBaseT {
  using M2 = typename Multiplier<M>::M2;

  template <typename T = M>
  explicit constexpr NonConstModBaseT(T x_ = 0) : x{static_cast<M>(x_)} {}

  static constexpr NonConstModBaseT mul_id() { return NonConstModBaseT{1}; }

  static void set_mod(M mod_) { store().set_mod(mod_); }

  template <typename T = M2>
  static constexpr std::enable_if_t<std::is_integral_v<T>, NonConstModBaseT>
  normalize(T x) {
    return NonConstModBaseT{reduce(x)};
  }

  static M mod() { return store().mod; }

  constexpr M get() const { return x; }

  constexpr NonConstModBaseT &operator+=(const NonConstModBaseT &other) {
    x += other.x;
    if (x >= mod()) {
      x -= mod();
    }
    return *this;
  }

  constexpr NonConstModBaseT operator+(const NonConstModBaseT &other) const {
    NonConstModBaseT copy = *this;
    return copy += other;
  }

  constexpr NonConstModBaseT &operator-=(const NonConstModBaseT &other) {
    x += mod() - other.x;
    if (x >= mod()) {
      x -= mod();
    }
    return *this;
  }

  constexpr NonConstModBaseT operator-() const {
    NonConstModBaseT copy{0};
    copy -= *this;
    return copy;
  }

  constexpr NonConstModBaseT operator-(const NonConstModBaseT &other) const {
    NonConstModBaseT copy = *this;
    return copy -= other;
  }

  constexpr NonConstModBaseT operator*=(const NonConstModBaseT &other) {
    x = reduce(static_cast<M2>(x) * static_cast<M2>(other.x));
    return *this;
  }

  constexpr NonConstModBaseT operator*(const NonConstModBaseT &other) const {
    NonConstModBaseT copy = *this;
    return copy *= other;
  }

  constexpr NonConstModBaseT inv() const {
    return x == 1 ? NonConstModBaseT{1}
                  : -NonConstModBaseT{mod() / x} *
                        NonConstModBaseT{mod() % x}.inv();
  }

private:
  struct Store {
    void set_mod(M mod_) { mod = mod_; }

    M reduce(M2 x) const {
      auto q = static_cast<long double>(x) / mod;
      auto r = x - q * mod;
      return r <= mod ? r + mod : r;
    }

    M mod;
  };

  static Store &store() { return Singleton<Store>::instance(); }

  static M reduce(M2 x_) { return store().reduce(x_); }

  M x;
};

} // namespace non_const_mod_details

namespace std {

template <typename M, int PHANTOM>
ostream &
operator<<(ostream &out,
           const non_const_mod_details::NonConstModBaseT<M, PHANTOM> &m) {
  return out << m.get();
}

} // namespace std

template <int PHANTOM = 0>
using NonConstModT = non_const_mod_details::NonConstModBaseT<uint32_t, PHANTOM>;
template <int PHANTOM = 0>
using NonConstMod64T =
    non_const_mod_details::NonConstModBaseT<uint64_t, PHANTOM>;
