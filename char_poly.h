#include <algorithm>
#include <vector>

template <typename Mod>
std::vector<Mod> char_poly(int n, std::vector<std::vector<Mod>> a) {
  // Hessenberg reduction
  for (int r = 1; r < n; r++) {
    // a[>= r][r - 1]
    int pivot = r;
    while (pivot < n && !a[pivot][r - 1].get()) {
      pivot++;
    }
    if (pivot < n) {
      std::swap(a[r], a[pivot]);
      for (int i = 0; i < n; i++) {
        std::swap(a[i][r], a[i][pivot]);
      }
      auto inv = a[r][r - 1].inv();
      for (int k = r + 1; k < n; k++) {
        if (a[k][r - 1].get()) {
          auto t = a[k][r - 1] * inv;
          for (int j = 0; j < n; j++) {
            a[k][j] -= t * a[r][j];
          }
          for (int i = 0; i < n; i++) {
            a[i][r] += t * a[i][k];
          }
        }
      }
    }
  }
  // char poly of Hessenberg matrix
  std::vector chi(n + 1, std::vector<Mod>{});
  chi[0].emplace_back(1);
  for (int i = 1; i <= n; i++) {
    // compute chi[i]
    chi[i].resize(i + 1);
    for (int j = 0; j < i; j++) {
      chi[i][j] -= a[i - 1][i - 1] * chi[i - 1][j];
      chi[i][j + 1] += chi[i - 1][j];
    }
    Mod coef{1};
    for (int k = 2; k <= i; k++) {
      coef *= a[i - k + 1][i - k];
      auto c = coef * a[i - k][i - 1];
      for (int j = 0; j <= i - k; j++) {
        chi[i][j] -= c * chi[i - k][j];
      }
    }
  }
  return chi[n];
}
