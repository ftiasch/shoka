#include "../polynomial_interpolation.h"
#include "../mod.h"

#include "gtest/gtest.h"

#include <random>

namespace {

static const int D = 100;
static const int N = 900000000;

using ModT = mod::ModT<998'244'353>;

ModT eval(const std::vector<ModT> &coef, ModT x) {
  ModT result{0};
  for (int i = coef.size(); i--;) {
    result = result * x + coef[i];
  }
  return result;
}

TEST(PolynomialInterpolation, Correctness) {
  std::mt19937 gen{0};
  std::vector<ModT> coef(D);
  for (int i = 0; i < D; ++i) {
    coef[i] = ModT(gen() % ModT::MOD);
  }
  std::vector<ModT> values(D);
  for (int i = 0; i < D; ++i) {
    values[i] = eval(coef, ModT(i));
  }
  PolynomialInterpolation<ModT> interpolate(D);
  ModT output = interpolate(values, ModT(N));
  ModT answer = eval(coef, ModT(N));
  ASSERT_EQ(output.get(), answer.get());
}

} // namespace
