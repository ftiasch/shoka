#include <vector>

template <typename Mod> struct Gaussian {
  static Mod det(std::vector<std::vector<Mod>> a) {
    const int n = a.size();
    Mod result(1);
    for (int j = 0; j < n; ++j) {
      int pivot = j;
      while (pivot < n && a[pivot][j].get() == 0) {
        pivot++;
      }
      if (pivot == n) {
        return Mod(0);
      }
      if (j < pivot) {
        result *= Mod(Mod::mod() - 1);
        std::swap(a[j], a[pivot]);
      }
      Mod normalizer = a[j][j].inv();
      for (int i = j + 1; i < n; ++i) {
        if (a[i][j].get()) {
          Mod scale = a[i][j] * normalizer;
          for (int k = j; k < n; ++k) {
            a[i][k] -= scale * a[j][k];
          }
        }
      }
      result *= a[j][j];
    }
    return result;
  }
};
