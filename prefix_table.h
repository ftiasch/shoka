#include <algorithm>

template <int N> struct PrefixTable {
  void compute(int n, const char *s) {
    z[0] = 0;
    for (int i = 1, j = 0; i < n; ++i) {
      z[i] = i < j + z[j] ? std::min(z[i - j], j + z[j] - i) : 0;
      while (i + z[i] < n && s[z[i]] == s[i + z[i]]) {
        z[i]++;
      }
      if (i + z[i] > j + z[j]) {
        j = i;
      }
    }
    z[0] = n;
  }

  int z[N];
};
