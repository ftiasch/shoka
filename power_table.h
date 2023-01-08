#include <vector>

template <typename Mod> struct PowerTable : public std::vector<Mod> {
  explicit PowerTable(int n, Mod a) : std::vector<Mod>(n) {
    (*this)[0] = Mod{1};
    for (int i = 1; i < n; ++i) {
      (*this)[i] = (*this)[i - 1] * a;
    }
  }
};
