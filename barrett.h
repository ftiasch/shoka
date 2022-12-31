#pragma once

#include "mod_wrapper.h"

#include <cstdint>
#include <limits>

namespace mod_details {

template <typename M> struct BarrettMultiplierT;

template <> struct BarrettMultiplierT<uint32_t> {
  using M2 = uint64_t;

  static constexpr M2 mul_hi(M2 x, M2 y) {
    return static_cast<__uint128_t>(x) * static_cast<__uint128_t>(y) >> 64;
  }
};

template <> struct BarrettMultiplierT<uint64_t> {
  using M2 = __uint128_t;

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

template <typename M_, int PHANTOM> struct BarrettModBaseT {
  using M = M_;
  using M2 = typename BarrettMultiplierT<M>::M2;
  static constexpr M MOD = 0;

  M mod() { return mod_; }

  M reduce(M2 x) {
    auto q = BarrettMultiplierT<M>::mul_hi(x, inv_mod);
    auto r = x - q * mod();
    return r >= mod() ? r - mod() : r;
  }

  void set_mod(M mod) {
    mod_ = mod;
    inv_mod = static_cast<M2>(-1) / mod;
  }

private:
  M mod_;
  M2 inv_mod;
};

template <int PHANTOM = 0>
using BarrettModT = ModWrapperT<BarrettModBaseT<uint32_t, PHANTOM>>;
template <int PHANTOM = 0>
using BarrettMod64T = ModWrapperT<BarrettModBaseT<uint64_t, PHANTOM>>;

} // namespace mod_details

using mod_details::BarrettMod64T;
using mod_details::BarrettModT;
