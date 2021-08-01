#include <vector>

template <typename ModT> struct Gaussian {
  using Matrix = std::vector<std::vector<ModT>>;

  static ModT det(Matrix a) {
    const int n = a.size();
    ModT result(1);
    for (int j = 0; j < n; ++j) {
      int pivot = j;
      while (pivot < n && a[pivot][j].get() == 0) {
        pivot++;
      }
      if (pivot == n) {
        return ModT(0);
      }
      if (j < pivot) {
        result *= ModT(ModT::MOD - 1);
        std::swap(a[j], a[pivot]);
      }
      ModT normalizer = a[j][j].inverse();
      for (int i = j + 1; i < n; ++i) {
        if (a[i][j].get()) {
          ModT scale = a[i][j] * normalizer;
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
