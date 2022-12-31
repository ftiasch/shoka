#pragma once

#include "singleton.h"

#include <iostream>

namespace mod_details {

template <typename Mod> struct ModWrapperT {
  using M = typename Mod::M;
  using M2 = typename Mod::M2;
  static constexpr M MOD = 0;

  static constexpr ModWrapperT mul_id() { return ModWrapperT{1}; }

  static constexpr ModWrapperT normalize(M2 x) {
    return ModWrapperT{x % mod()};
  }

  static void set_mod(M mod) {
    if constexpr (Mod::MOD == 0) {
      instance().set_mod(mod);
    }
  }

  template <typename T = M>
  explicit constexpr ModWrapperT(T x_ = 0) : x{static_cast<M>(x_)} {}

  static constexpr M mod() {
    if constexpr (Mod::MOD) {
      return Mod::MOD;
    }
    return instance().mod();
  }

  constexpr M get() const { return x; }

  bool operator==(const ModWrapperT &other) const {
    return get() == other.get();
  }

  bool operator!=(const ModWrapperT &other) const { return !(*this == other); }

  constexpr ModWrapperT &operator+=(const ModWrapperT &other) {
    x += other.x;
    if (x >= mod()) {
      x -= mod();
    }
    return *this;
  }

  constexpr ModWrapperT operator+(const ModWrapperT &other) const {
    ModWrapperT copy = *this;
    return copy += other;
  }

  constexpr ModWrapperT &operator-=(const ModWrapperT &other) {
    x += mod() - other.x;
    if (x >= mod()) {
      x -= mod();
    }
    return *this;
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

  constexpr ModWrapperT operator*=(const ModWrapperT &other) {
    auto prod = static_cast<M2>(x) * static_cast<M2>(other.x);
    if constexpr (Mod::MOD) {
      x = Mod::reduce(prod);
    } else {
      x = instance().reduce(prod);
    }
    return *this;
  }

  constexpr ModWrapperT operator*(const ModWrapperT &other) const {
    ModWrapperT copy = *this;
    return copy *= other;
  }

  constexpr ModWrapperT inv() const {
    return x == 1 ? ModWrapperT{1}
                  : -ModWrapperT{mod() / x} * ModWrapperT{mod() % x}.inv();
  }

private:
  static constexpr Mod &instance() { return Singleton<Mod>::instance(); }

  M x;
};

} // namespace mod_details

namespace std {

template <typename Mod>
ostream &operator<<(ostream &out, const mod_details::ModWrapperT<Mod> &m) {
  return out << m.get();
}

} // namespace std
