#pragma once

#include "poly_div.h"

template <typename Poly> struct PolyLog : public PolyOp<Poly, PolyLog> {
  using Base = PolyOp<Poly, PolyLog>;
  SHOKA_HELPER_USING_POLY_OP;

  explicit PolyLog() : inv{Mod{0}, Mod{1}}, div{} {}

  Poly operator()(const Poly &f) { return Base::template single<4>(f); }

  void _(int n, Mod *out, const Mod *f) {
    if (f[0].get() != 1) {
      throw std::invalid_argument("[x^0] f != 1");
    }
    Poly::assert_power_of_two(n);
    Poly::reserve(n);
    // log f = \int f' / f
    const auto d_f = Poly::template raw_buffer<3>();
    d_f[n - 1] = Mod{0};
    for (int i = 1; i < n; ++i) {
      d_f[i - 1] = Mod{i} * f[i];
    }
    div._(n, out, d_f, f);
    for (int i = n; i-- > 1;) {
      out[i] = out[i - 1] * cached_inv(i);
    }
    out[0] = Mod{0};
  }

  Mod cached_inv(int n) {
    if (static_cast<int>(inv.size()) <= n) {
      int old_size = inv.size();
      inv.resize(n + 1);
      for (int i = old_size; i <= n; ++i) {
        inv[i] = -Mod{Mod::mod() / i} * inv[Mod::mod() % i];
      }
    }
    return inv[n];
  }

private:
  PolyDiv<Poly> div;
  std::vector<Mod> inv;
};
