#include <cmath>
#include <vector>

struct PrimeCount {
  using Long = long long;

  PrimeCount(Long n_)
      : n(n_), sqrt_n(std::sqrt(n)), map_small(sqrt_n + 1),
        map_large(sqrt_n + 1) {
    std::vector<bool> is_prime(sqrt_n + 1, true);
    primes.push_back(1);
    for (int d = 2; d <= sqrt_n; ++d) {
      if (is_prime[d]) {
        primes.push_back(d);
      }
      for (int i = 1; i < primes.size() && d * primes[i] <= sqrt_n; ++i) {
        is_prime[d * primes[i]] = false;
        if (d % primes[i] == 0) {
          break;
        }
      }
    }
    // find all possible value of n / i
    pi.reserve(2 * sqrt_n);
    std::vector<Long> values(2 * sqrt_n);
    int m = 0;
    for (Long i = 1; i <= n;) {
      Long d = n / i;
      pi[m] = d - 1;
      values[m] = d;
      (d <= sqrt_n ? map_small[d] : map_large[n / d]) = m++;
      i = n / d + 1;
    }
    for (int k = 1; k < primes.size(); ++k) {
      Long bound = 1LL * primes[k] * primes[k];
      for (int i = 0; values[i] >= bound; ++i) {
        pi[i] -= pi[id(values[i] / primes[k])] - (k - 1);
      }
    }
  }

  int id(Long d) const {
    // there exists an i where n / i = d
    return d <= sqrt_n ? map_small[d] : map_large[n / d];
  }

  // returns \pi(n / d)
  Long operator()(Long d = 1) const { return pi[id(n / d)]; }

  Long n;
  int sqrt_n;
  std::vector<int> primes, map_small, map_large;
  std::vector<Long> pi;
};
