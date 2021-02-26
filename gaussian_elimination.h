Mod power(Mod a, int n) {
  Mod result{1};
  while (n) {
    if (n & 1) {
      result *= a;
    }
    a *= a;
    n >>= 1;
  }
  return result;
}

Mod inverse(Mod a) { return power(a, Mod::MOD - 2); }

using Matrix = std::vector<std::vector<Mod>>;

std::vector<Mod> gaussian_eliminate(int n, Matrix coef) {
  for (int j = 0; j < n; ++j) {
    int pivot = j;
    while (pivot < n && coef[pivot][j].get() == 0) {
      pivot++;
    }
    assert(pivot < n);
    std::swap(coef[j], coef[pivot]);
    {
      Mod normalizer = inverse(coef[j][j]);
      for (int k = j; k <= n; ++k) {
        coef[j][k] *= normalizer;
      }
    }
    for (int i = 0; i < n; ++i) {
      if (i != j) {
        Mod scale = coef[i][j];
        for (int k = j; k <= n; ++k) {
          coef[i][k] -= scale * coef[j][k];
        }
      }
    }
  }
  std::vector<Mod> result(n);
  for (int i = 0; i < n; ++i) {
    result[i] = coef[i][n];
  }
  return result;
}
