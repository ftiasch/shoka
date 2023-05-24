#include "binpow.h"
#include "sieve.h"

#include <vector>

template <typename Mod> struct FixedPowerTable : public std::vector<Mod> {
  explicit FixedPowerTable(int n, int k) : std::vector<Mod>(n) {
    (*this)[0] = Mod{0};
    if (1 < n) {
      (*this)[1] = Mod{1};
    }
    PrimeGen primes(n);
    for (int i = 2; i < n; ++i) {
      auto p = primes.min_div(i);
      (*this)[i] = p == i ? binpow(Mod{i}, k) : (*this)[i / p] * (*this)[p];
    }
  }
};
