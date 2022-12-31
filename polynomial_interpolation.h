#include "inv_table.h"

#include <vector>

template <typename Mod> struct PolynomialInterpolation {
  explicit PolynomialInterpolation(int maxd) : inverse(maxd) {}

  Mod operator()(std::vector<Mod> values, Mod n) const {
    int d = static_cast<int>(values.size()) - 1;
    // d < maxd
    {
      Mod b{1};
      for (int i = 1; i <= d; ++i) {
        b = b * (n - Mod{i - 1}) * inverse[i];
        values[i] *= b;
      }
    }
    Mod result = values[d];
    {
      Mod b{1};
      for (int i = d - 1; i >= 0; --i) {
        b = (Mod{0} - b) * inverse[d - i] * (n - Mod{i + 1});
        result += values[i] * b;
      }
    }
    return result;
  }

  InvTable<Mod> inverse;
};
