#pragma once

#include "mod_wrapper.h"

#include <cstdint>
#include <limits>

namespace mod {

template <typename M_, M_ MOD_> struct ModBaseT {
  using M = M_;
  static constexpr M MOD = MOD_;

  static_assert((MOD - 1) <= (std::numeric_limits<M_>::max() >> 1));

private:
  using M2 = m2_t<M>;

public:
  static constexpr M reduce(M2 x) { return x % MOD; }
};

template <uint64_t M> using Mod64T = ModWrapperT<ModBaseT<uint64_t, M>>;
template <uint32_t M> using ModT = ModWrapperT<ModBaseT<uint32_t, M>>;

} // namespace mod

using mod::Mod64T;
using mod::ModT;
