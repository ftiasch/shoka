#pragma once

#include "mod_wrapper.h"

namespace mod {

template <typename M_, int PHANTOM> struct BarrettBaseT {
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

template <int PHANTOM = 0>
using Barrett64T = ModWrapperT<BarrettBaseT<uint64_t, PHANTOM>>;
template <int PHANTOM = 0>
using BarrettT = ModWrapperT<BarrettBaseT<uint32_t, PHANTOM>>;

} // namespace mod

using mod::Barrett64T;
using mod::BarrettT;
