#pragma once

#include "mod_wrapper.h"

#include <cstdint>
#include <limits>

namespace mod_details {

template <typename M_, typename M2_, M_ MOD_> struct ModBaseT {
  using M = M_;
  using M2 = M2_;
  static constexpr M MOD = MOD_;

  static_assert(MOD <= (std::numeric_limits<M_>::max() >> 1));

  static constexpr M mod() { return MOD; }

  static constexpr M reduce(M2 x) { return x % MOD; }

#ifdef SHOKA_TESTING
  static void set_mod(M_) {}
#endif
};

template <uint32_t MOD>
using ModT = ModWrapperT<ModBaseT<uint32_t, uint64_t, MOD>>;
template <uint64_t MOD>
using Mod64T = ModWrapperT<ModBaseT<uint64_t, __uint128_t, MOD>>;

} // namespace mod_details

using mod_details::Mod64T;
using mod_details::ModT;
