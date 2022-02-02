#include <vector>

template <typename ModT> struct PolynomialInterpolation {
  PolynomialInterpolation(int maxd) : inverse(maxd) {
    inverse[1] = ModT{1};
    for (int i = 2; i < maxd; ++i) {
      inverse[i] = ModT(ModT::MOD - ModT::MOD / i) * inverse[ModT::MOD % i];
    }
  }

  ModT operator()(std::vector<ModT> values, ModT n) const {
    int d = static_cast<int>(values.size()) - 1;
    // d < maxd
    {
      ModT b{1};
      for (int i = 1; i <= d; ++i) {
        b = b * (n - ModT(i - 1)) * inverse[i];
        values[i] *= b;
      }
    }
    ModT result = values[d];
    {
      ModT b{1};
      for (int i = d - 1; i >= 0; --i) {
        b = (ModT{0} - b) * inverse[d - i] * (n - ModT(i + 1));
        result += values[i] * b;
      }
    }
    return result;
  }

  std::vector<ModT> inverse;
};
