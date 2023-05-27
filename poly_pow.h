
#pragma once

#include "poly_exp.h"
#include "poly_log.h"

template <typename Poly> struct PolyPow {
  using Mod = typename Poly::Mod;

  Poly operator()(const Poly &f, uint64_t n) {
    int min_deg = 0;
    while (min_deg < f.size() && f[min_deg] == Mod{0}) {
      min_deg++;
    }
    if (min_deg * n >= f.size()) {
      return Poly(f.size());
    }
    // x^{min_deg * n}
    int trunc_len = f.size() - min_deg * n;
    auto c0 = f[min_deg];
    auto inv_c0 = c0.inv();
    Poly trunc_f(trunc_len);
    for (int i = 0; i < trunc_len; i++) {
      trunc_f[i] = inv_c0 * f[min_deg + i];
    }
    auto log_f = log(trunc_f);
    auto mod_n = Mod::normalize(n);
    for (auto &c : log_f) {
      c *= mod_n;
    }
    auto exp_f = exp(log_f);
    Poly result(f.size());
    auto pow_c0 = binpow(c0, n);
    for (int i = 0; i < trunc_len; i++) {
      result[min_deg * n + i] = pow_c0 * exp_f[i];
    }
    return result;
  }

private:
  PolyLog<Poly> log;
  PolyExp<Poly> exp;
};
