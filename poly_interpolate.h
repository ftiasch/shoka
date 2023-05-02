#include "dyn_inv_table.h"

#include <vector>

// x^n mod (x - 1)^{k + 1}
template <typename Mod> std::vector<Mod> mod_pow_of_minus_1(Mod n, int k) {
  std::vector<Mod> coef(k + 1);
  coef[0] = Mod{1};
  for (int i = 0; i < k; i++) {
    coef[i + 1] = coef[i] * (n - Mod{i}) * inv(Mod{i + 1});
  }
  Mod r{1}, b{n - Mod{k}}, c{1};
  for (int i = k; i >= 0; i--, b += Mod{1}, c += Mod{1}) {
    coef[i] *= r;
    r = -r * b * inv(c);
  }
  return coef;
}

template <typename Mod>
static Mod poly_interpolate1(const std::vector<Mod> &values, Mod n) {
  auto d = static_cast<int>(values.size()) - 1;
  auto coef = mod_pow_of_minus_1(n, d);
  Mod result{0};
  for (int i = d; i >= 0; i--) {
    result += coef[i] * values[i];
  }
  return result;
}
