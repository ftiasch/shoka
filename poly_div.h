#pragma once

#include "poly_inv.h"

template <typename NTT> struct PolyDiv {
  using Factory = PolyFactory<NTT>;
  using Mod = typename Factory::Mod;
  using Poly = typename Factory::Poly;

  explicit PolyDiv(std::shared_ptr<Factory> factory_)
      : factory{factory_}, inv{factory} {}

  Poly operator()(const Poly &f, const Poly &g) {
    int max_size = std::max(f.size(), g.size());
    int n = Factory::min_power_of_two(max_size);
    factory->reserve(n);
    Poly out = factory->make(n);
    Mod *const fb = factory->template raw_buffer<3>();
    Mod *const gb = factory->template raw_buffer<4>();
    Factory::copy_and_fill0(n, fb, f);
    Factory::copy_and_fill0(n, gb, g);
    _(n, out.data(), fb, gb);
    out.resize(max_size);
    return out;
  }

  void _(int n, Mod *out, const Mod *f, const Mod *g) {
    if (n == 1) {
      out[0] = f[0] * g[0].inverse();
    } else {
      Factory::assert_power_of_two(n);
      factory->reserve(n);
      Mod *const b0 = factory->template raw_buffer<0>();
      Mod *const b1 = factory->template raw_buffer<1>();
      Mod *const inv_g = factory->template raw_buffer<2>();
      int m = n >> 1;
      inv._(m, inv_g, g);
      std::fill(inv_g + m, inv_g + n, Mod(0));
      NTT::dif(n, inv_g);
      Factory::copy_and_fill0(n, b0, m, f);
      NTT::dif(n, b0);
      const Mod inv_n = Mod(n).inverse();
      Factory::dot_product_and_dit(n, inv_n, b0, b0, inv_g);

      std::copy(b0, b0 + m, out);

      std::fill(b0 + m, b0 + n, Mod(0));
      NTT::dif(n, b0);
      std::copy(g, g + n, b1);
      NTT::dif(n, b1);
      Factory::dot_product_and_dit(n, inv_n, b0, b0, b1);
      std::fill(b0, b0 + m, Mod(0));
      for (int i = m; i < n; ++i) {
        b0[i] -= f[i];
      }
      NTT::dif(n, b0);
      Factory::dot_product_and_dit(n, inv_n, b0, b0, inv_g);
      for (int i = m; i < n; ++i) {
        out[i] = Mod(0) - b0[i];
      }
    }
  }

  std::shared_ptr<Factory> factory;
  PolyInv<NTT> inv;
};
