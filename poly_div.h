#pragma once

#include "snippets/min_pow_of_two.h"

#include "poly_inv.h"

template <typename Poly> struct PolyDiv {
  using Mod = typename Poly::Mod;

  Poly operator()(const Poly &f, const Poly &g) {
    int max_size = std::max(f.size(), g.size());
    int n = min_pow_of_two(max_size);
    Poly::reserve(n);
    Poly out(n);
    const auto fb = Poly::template raw_buffer<3>();
    const auto gb = Poly::template raw_buffer<4>();
    Poly::copy_and_fill0(n, fb, f);
    Poly::copy_and_fill0(n, gb, g);
    _(n, out.data(), fb, gb);
    out.resize(max_size);
    return out;
  }

  void _(int n, Mod *out, const Mod *f, const Mod *g) {
    if (n == 1) {
      out[0] = f[0] * g[0].inv();
    } else {
      Poly::assert_power_of_two(n);
      Poly::reserve(n);
      const auto b0 = Poly::template raw_buffer<0>();
      const auto b1 = Poly::template raw_buffer<1>();
      const auto inv_g = Poly::template raw_buffer<2>();
      int m = n >> 1;
      inv._(m, inv_g, g);
      std::fill(inv_g + m, inv_g + n, Mod{0});
      Poly::dif(n, inv_g);
      Poly::copy_and_fill0(n, b0, m, f);
      Poly::dif(n, b0);
      Poly::dot_product_and_dit(n, b0, b0, inv_g);

      std::copy(b0, b0 + m, out);

      std::fill(b0 + m, b0 + n, Mod{0});
      Poly::dif(n, b0);
      std::copy(g, g + n, b1);
      Poly::dif(n, b1);
      Poly::dot_product_and_dit(n, b0, b0, b1);
      std::fill(b0, b0 + m, Mod{0});
      for (int i = m; i < n; ++i) {
        b0[i] -= f[i];
      }
      Poly::dif(n, b0);
      Poly::dot_product_and_dit(n, b0, b0, inv_g);
      for (int i = m; i < n; ++i) {
        out[i] = Mod{0} - b0[i];
      }
    }
  }

private:
  PolyInv<Poly> inv;
};
