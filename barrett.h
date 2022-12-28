#pragma once

#include "singleton.h"

#include <cstdint>
#include <iostream>
#include <limits>
#include <type_traits>

template <int PHANTOM = 0> struct BarretMod {
  template <typename T = uint32_t>
  explicit constexpr BarretMod(T x_ = 0) : x{static_cast<uint32_t>(x_)} {}

  static constexpr BarretMod mul_id() { return BarretMod{1}; }

  static void set_mod(uint32_t mod_) { storage().set_mod(mod_); }

  template <typename T = uint64_t>
  static constexpr std::enable_if_t<std::is_integral_v<T>, BarretMod>
  normalize(T x) {
    return BarretMod{reduce(x)};
  }

  constexpr uint32_t get() const { return x; }

  constexpr BarretMod &operator+=(const BarretMod &other) {
    x += other.x;
    if (x >= mod()) {
      x -= mod();
    }
    return *this;
  }

  constexpr BarretMod operator+(const BarretMod &other) const {
    BarretMod copy = *this;
    return copy += other;
  }

  constexpr BarretMod &operator-=(const BarretMod &other) {
    x += mod() - other.x;
    if (x >= mod()) {
      x -= mod();
    }
    return *this;
  }

  constexpr BarretMod operator-() const {
    BarretMod copy{0};
    copy -= *this;
    return copy;
  }

  constexpr BarretMod operator-(const BarretMod &other) const {
    BarretMod copy = *this;
    return copy -= other;
  }

  constexpr BarretMod operator*=(const BarretMod &other) {
    x = reduce(static_cast<uint64_t>(x) * static_cast<uint64_t>(other.x));
    return *this;
  }

  constexpr BarretMod operator*(const BarretMod &other) const {
    BarretMod copy = *this;
    return copy *= other;
  }

private:
  struct Storage {
    void set_mod(uint32_t mod_) {
      mod = mod_;
      inv_mod = static_cast<uint64_t>(-1) / mod + 1;
    }

    uint32_t reduce(uint64_t x) const {
      auto q = static_cast<uint64_t>(static_cast<__uint128_t>(x) *
                                         static_cast<__uint128_t>(inv_mod) >>
                                     64);
      auto r = x - q * mod;
      return mod <= r ? r - mod : r;
    }

    uint32_t mod;
    uint64_t inv_mod;
  };

  static Storage &storage() { return Singleton<Storage>::instance(); }

  static uint32_t mod() { return storage().mod; }

  static uint32_t reduce(uint64_t x_) { return storage().reduce(x_); }

  uint32_t x;
};

namespace std {

template <int PHANTOM>
ostream &operator<<(ostream &out, const BarretMod<PHANTOM> &m) {
  return out << m.get();
}

} // namespace std
