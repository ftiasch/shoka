#include "char_poly.h"

#include <array>
#include <vector>

template <typename Mod>
std::vector<Mod> det_f2(int n, std::vector<std::vector<std::array<Mod, 2>>> a) {
  // a[0] + a[1] * x
  int shift = 0;
  Mod scale{1};
  for (int j = 0; j < n; j++) {
    // Make a[j][j][1] == 1
    int pivot;
    while (true) {
      pivot = j;
      while (pivot < n && !a[j][pivot][1].get()) {
        pivot++;
      }
      if (pivot < n) {
        break;
      }
      if (++shift > n) {
        return std::vector<Mod>(n + 1, Mod{0});
      }
      // *= x
      for (int k = 0; k < n; k++) {
        a[j][k] = {Mod{0}, a[j][k][0]};
      }
      for (int k = 0; k < j; k++) {
        auto t = a[j][k][1];
        if (t.get()) {
          for (int p = 0; p < n; p++) {
            a[j][p][0] -= t * a[k][p][0];
            a[j][p][1] -= t * a[k][p][1];
          }
        }
      }
    }
    if (j < pivot) {
      scale = -scale;
      for (int i = 0; i < n; i++) {
        std::swap(a[i][j], a[i][pivot]);
      }
    }
    scale *= a[j][j][1];
    {
      auto t = a[j][j][1].inv();
      for (int k = 0; k < n; k++) {
        a[j][k][0] *= t;
        a[j][k][1] *= t;
      }
    }
    for (int i = 0; i < n; i++) {
      auto t = a[i][j][1];
      if (i != j && t.get()) {
        for (int p = 0; p < n; p++) {
          a[i][p][0] -= t * a[j][p][0];
          a[i][p][1] -= t * a[j][p][1];
        }
      }
    }
  }
  std::vector b(n, std::vector<Mod>(n));
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      b[i][j] = -a[i][j][0];
    }
  }
  auto chi = char_poly(n, b);
  std::vector<Mod> result(n + 1);
  for (int i = 0; i + shift <= n; i++) {
    result[i] = scale * chi[i + shift];
  }
  return result;
}
