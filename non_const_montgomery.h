#include "mod_details.h"

#include <cstdint>

namespace montgomery {

using u32 = uint32_t;
using u64 = uint64_t;

struct MontgomeryNonConst {
  static void set_mod(u32 mod_) {
    mod = mod_;
    mod2 = mod << 1;
    r = rpower(mod, 32);
    r2 = rpower(mod, 64);
    inv = details::mont_modinv<u32>(mod, 5);
  }

  explicit MontgomeryNonConst(u32 x = 0) : x(mont_multiply(x, r2)) {}

  u32 get() const {
    u32 y = reduce(x);
    return y >= mod ? y - mod : y;
  }

  MontgomeryNonConst &operator+=(const MontgomeryNonConst &other) {
    x += other.x;
    if (x >= mod2) {
      x -= mod2;
    }
    return *this;
  }

  MontgomeryNonConst operator+(const MontgomeryNonConst &other) const {
    auto copy = *this;
    return copy += other;
  }

  MontgomeryNonConst &operator-=(const MontgomeryNonConst &other) {
    x += mod2 - other.x;
    if (x >= mod2) {
      x -= mod2;
    }
    return *this;
  }

  MontgomeryNonConst operator-(const MontgomeryNonConst &other) const {
    auto copy = *this;
    return copy -= other;
  }

  MontgomeryNonConst operator*=(const MontgomeryNonConst &other) {
    x = mont_multiply(x, other.x);
    return *this;
  }

  MontgomeryNonConst operator*(const MontgomeryNonConst &other) const {
    auto copy = *this;
    return copy *= other;
  }

private:
  static constexpr u32 rpower(u32 mod, int n) {
    u32 result = 1;
    for (int i = 0; i < n; ++i) {
      result += result;
      if (result >= mod) {
        result -= mod;
      }
    }
    return result;
  }

  static u32 mont_multiply(u32 x, u32 y) {
    return reduce(static_cast<u64>(x) * y);
  }

  static u32 reduce(u32 x) { return reduce(static_cast<u64>(x)); }

  static u64 reduce(u64 x) {
    u64 y = (((x & UINT32_MAX) * inv) & UINT32_MAX) * mod;
    return (x + y) >> 32U;
  }

  u32 x;

  static u32 mod;
  static u32 mod2;
  static u32 r;
  static u32 r2;
  static u32 inv;
};

} // namespace montgomery

#define DECLARE_NON_CONST_MONTGOMERY(name)                                     \
  using name = montgomery::MontgomeryNonConst;                                 \
  uint32_t name::mod;                                                          \
  uint32_t name::mod2;                                                         \
  uint32_t name::r;                                                            \
  uint32_t name::r2;                                                           \
  uint32_t name::inv;
