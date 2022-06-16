#pragma once

#include "poly_op.h"

template <typename NTT> struct PolyInv : public PolyOp<PolyInv, NTT> {
  using Base = PolyOp<PolyInv, NTT>;
  using Base::factory;
  using typename Base::Factory;
  using typename Base::Mod;
  using typename Base::Poly;

  Poly operator()(const Poly &f) { return Base::template single<2>(f); }

  void _(int n, Mod *out, const Mod *f) {
    Factory::assert_power_of_two(n);
    factory->reserve(n);
    Mod *const b0 = factory->template raw_buffer<0>();
    Mod *const b1 = factory->template raw_buffer<1>();
    std::fill(out, out + n, Mod(0));
    out[0] = f[0].inverse();
    Mod inv_m(1);
    for (int m = 2; m <= n; m <<= 1) {
      Factory::copy_and_fill0(m, b0, n, f);
      NTT::dif(m, b0);
      std::copy(out, out + m, b1);
      NTT::dif(m, b1);
      inv_m *= Mod(2).inverse();
      Factory::dot_product_and_dit(m, inv_m, b0, b0, b1);
      std::fill(b0, b0 + (m >> 1), Mod(0));
      NTT::dif(m, b0);
      Factory::dot_product_and_dit(m, inv_m, b0, b0, b1);
      for (int i = m >> 1; i < m; ++i) {
        out[i] = Mod(0) - b0[i];
      }
    }
  }
};
