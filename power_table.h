#include <vector>

template <typename ModT> struct PowerTable : std::vector<ModT> {
  PowerTable(int n, ModT a) : std::vector<ModT>(n) {
    (*this)[0] = ModT{1};
    for (int i = 1; i < n; ++i) {
      (*this)[i] = (*this)[i - 1] * a;
    }
  }
};
