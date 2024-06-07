#pragma once

#include "snippets/min_pow_of_two.h"

#include <vector>

template <typename Poly> struct PolyCompose {
  using Mod = typename Poly::Mod;

  // F(G(z))
  Poly operator()(const Poly &f, const Poly &g) {
    int n = f.size();
    int n2 = min_pow_of_two(n + n - 1);
    Poly::reserve(n2);
    int sqrt_n = 1;
    while (sqrt_n * sqrt_n < n) {
      sqrt_n++;
    }
    // sqrt_n * sqrt_n >= n
    const auto dif_g = Poly::template raw_buffer<0>();
    Poly::copy_and_fill0(n2, dif_g, g);
    Poly::dif(n2, dif_g);
    std::vector<Mod> coef(sqrt_n * n);
    // k == 0
    for (int i = 0, offset = 0; i < n; i += sqrt_n, offset += n) {
      coef[offset] += f[i];
    }
    const auto pow_g = Poly::template raw_buffer<1>();
    Poly::copy_and_fill0(n2, pow_g, std::min<int>(n, g.size()), g.data());
    for (int k = 1; k < sqrt_n; k++) {
      for (int i = k, offset = 0; i < n; i += sqrt_n, offset += n) {
        for (int j = 0; j < n; j++) {
          coef[offset + j] += f[i] * pow_g[j];
        }
      }
      Poly::dif(n2, pow_g);
      Poly::dot_product_and_dit(n2, pow_g, pow_g, dif_g);
      std::fill(pow_g + n, pow_g + n2, Mod{0});
    }
    const auto dif_pow_g = pow_g;
    Poly::dif(n2, dif_pow_g);
    const auto res = dif_g;
    std::fill(res, res + n2, Mod{0});
    for (int offset = sqrt_n * n, k = sqrt_n; offset -= n, k--;) {
      Poly::dif(n2, res);
      Poly::dot_product_and_dit(n2, res, res, dif_pow_g);
      std::fill(res + n, res + n2, Mod{0});
      for (int j = 0; j < n; j++) {
        res[j] += coef[offset + j];
      }
    }
    return Poly{res, res + n};
  }
};
