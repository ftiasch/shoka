#include <bits/stdc++.h>

namespace montgomery {

using u64 = uint64_t;
using u32 = uint32_t;

struct u128 {
  static u128 multiply(u64 x, u64 y) {
#ifdef U128_ASM
    u64 high, low;
    __asm__("mulq %3\n\t" : "=d"(high), "=a"(low) : "%a"(x), "rm"(y) : "cc");
    return {high, low};
#else
    uint64_t a = x & UINT32_MAX, b = x >> 32u, c = y & UINT32_MAX, d = y >> 32u,
             ac = a * c, bc = b * c, ad = a * d,
             z = (bc & UINT32_MAX) + (ad & UINT32_MAX) + (ac >> 32u);
    return {b * d + (bc >> 32u) + (ad >> 32u) + (z >> 32u),
            (z & UINT32_MAX) << 32u | (ac & UINT32_MAX)};
#endif
  }

  u64 high, low;
};

template <u64 MOD> struct MontgomeryT {
  static_assert(MOD <= std::numeric_limits<u64>::max() >> 1, "");

  static u64 from_monty(u64 x) { return reduce(u128{0, x}); }

  static u64 multiply(u64 x, u64 y) { return reduce(u128::multiply(x, y)); }

  static u64 to_monty(u64 x) { return multiply(x, R2); }

  static u64 inverse(u64 a) {
    u64 result = R;
    u64 n = MOD - 2;
    while (n) {
      if (n & 1) {
        result = multiply(result, a);
      }
      a = multiply(a, a);
      n >>= 1;
    }
    return result;
  }

private:
  static constexpr u64 modinv() {
    u64 result = 1;
    for (int i = 0; i < 6; ++i) {
      result *= 2 - MOD * result;
    }
    return -result;
  }

  static constexpr u64 rpower(int n) {
    u64 result = 1;
    for (int i = 0; i < n; ++i) {
      result += result;
      if (result >= MOD) {
        result -= MOD;
      }
    }
    return result;
  }

  static u64 reduce(u128 x) {
    u128 y = u128::multiply(u128::multiply(x.low, INV).low, MOD);
    u64 result = x.high + y.high + (x.low > ~y.low);
    if (result >= MOD) {
      result -= MOD;
    }
    return result;
  }

  static const u64 INV = modinv();
  static const u64 R = rpower(64);
  static const u64 R2 = rpower(128);

  u64 x;
};

} // namespace montgomery
