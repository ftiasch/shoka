#pragma once

#include "mod_wrapper.h"

#include <cstdint>
#include <iostream>
#include <limits>
#include <type_traits>

namespace mod_details {

template <typename M> struct NonConstMultiplierT;

template <> struct NonConstMultiplierT<uint32_t> { using M2 = uint64_t; };

template <> struct NonConstMultiplierT<uint64_t> { using M2 = __uint128_t; };

template <typename M_, int PHANTOM> struct NonConstModBaseT {
  using M = M_;
  using M2 = typename NonConstMultiplierT<M_>::M2;
  static constexpr M MOD = 0;

  M mod() { return mod_; }

  M reduce(M2 x) { return x % mod_; }

  void set_mod(M mod) { mod_ = mod; }

private:
  M mod_;
};

template <int PHANTOM = 0>
using NonConstModT = ModWrapperT<NonConstModBaseT<uint32_t, PHANTOM>>;
template <int PHANTOM = 0>
using NonConstMod64T = ModWrapperT<NonConstModBaseT<uint64_t, PHANTOM>>;

} // namespace mod_details

using mod_details::NonConstMod64T;
using mod_details::NonConstModT;
