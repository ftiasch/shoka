#pragma once

#include "sieve.h"

#include <array>
#include <vector>

struct QuickGcd {
  explicit QuickGcd(int m)
      : sqrt_m{isqrt(m)}, primes{m}, f3(std::max(m, 2)),
        gcd(sqrt_m, std::vector<int>(sqrt_m)) {
    f3[1] = {1, 1, 1};
    for (int i = 2; i < m; i++) {
      auto p = primes.min_div(i);
      f3[i] = f3[i / p];
      f3[i][0] *= p;
      for (int t = 0; t < 2; t++) {
        if (f3[i][t] > f3[i][t + 1]) {
          std::swap(f3[i][t], f3[i][t + 1]);
        }
      }
    }
    while (sqrt_m * sqrt_m < m) {
      sqrt_m++;
    }
    for (int i = 1; i < sqrt_m; i++) {
      gcd[i][0] = gcd[0][i] = i;
      for (int j = 1; j <= i; j++) {
        gcd[i][j] = gcd[j][i] = gcd[i - j][j];
      }
    }
  }

  int operator()(int a, int b) {
    int result = 1;
    for (auto &&d : f3[a]) {
      auto g = d < sqrt_m ? gcd[d][b % d] : (b % d == 0 ? d : 1);
      result *= g;
      b /= g;
    }
    return result;
  }

private:
  static constexpr int isqrt(int m) {
    int s = 1;
    while (s * s < m) {
      s++;
    }
    return s;
  }

  int sqrt_m;
  PrimeGen primes;
  std::vector<std::array<int, 3>> f3;
  std::vector<std::vector<int>> gcd;
};
