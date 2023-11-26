#include "binom_sum.h"

template <typename Mod>
static Mod poly_interpolate(const std::vector<Mod> &values, Mod n) {
  auto d = static_cast<int>(values.size()) - 1;
  auto coef = BinomialSum<Mod>::monomial_mod(n, d);
  Mod result{0};
  for (int i = d; i >= 0; i--) {
    result += coef[i] * values[i];
  }
  return result;
}
