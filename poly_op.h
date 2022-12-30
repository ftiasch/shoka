#pragma once

#include "poly.h"

template <typename Poly, template <typename> class Impl> struct PolyOp {
  using Mod = typename Poly::Mod;
  using Ntt = typename Poly::Ntt;
  using Factory = typename Poly::Factory;

  template <int index> Poly single(const Poly &f) {
    int n = Ntt::min_power_of_two(f.size());
    factory().reserve(n);
    Poly out(n);
    auto b2 = factory().template raw_buffer<index>();
    Factory::copy_and_fill0(n, b2, f);
    static_cast<Impl<Poly> *>(this)->_(n, out.data(), b2);
    out.resize(f.size());
    return out;
  }

  static typename Poly::Factory &factory() { return Poly::factory(); }
};

#define SHOKA_HELPER_USING_POLY_OP                                             \
  using Mod = typename Base::Mod;                                              \
  using Ntt = typename Base::Ntt;                                              \
  using Factory = typename Base::Factory;                                      \
  using Base::factory
