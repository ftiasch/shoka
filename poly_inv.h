#pragma once

#include "poly_op.h"

template <typename Poly> struct PolyInv : public PolyOp<Poly, PolyInv> {
  using Base = PolyOp<Poly, PolyInv>;
  SHOKA_HELPER_USING_POLY_OP;

  Poly operator()(const Poly &f) { return this->template single<2>(f); }

  void _(int n, Mod *out, const Mod *f) {
    Ntt::assert_power_of_two(n);
    Poly::reserve(n);
    const auto b0 = Poly::template raw_buffer<0>();
    const auto b1 = Poly::template raw_buffer<1>();
    std::fill(out, out + n, Mod{0});
    out[0] = f[0].inv();
    Mod inv_m(1);
    for (int m = 2; m <= n; m <<= 1) {
      Poly::copy_and_fill0(m, b0, n, f);
      Ntt::dif(m, b0);
      std::copy(out, out + m, b1);
      Ntt::dif(m, b1);
      inv_m *= Mod{2}.inv();
      Poly::dot_product_and_dit(m, inv_m, b0, b0, b1);
      std::fill(b0, b0 + (m >> 1), Mod{0});
      Ntt::dif(m, b0);
      Poly::dot_product_and_dit(m, inv_m, b0, b0, b1);
      for (int i = m >> 1; i < m; ++i) {
        out[i] = Mod{0} - b0[i];
      }
    }
  }
};
