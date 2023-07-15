#include "debug.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>

template <uint64_t N> struct AsmFactor {
  using Factorization = std::vector<std::pair<uint64_t, int>>;

  Factorization operator()(uint64_t n) const {
    assert(n <= N);
    Factorization result;
#pragma GCC unroll 3401
    for (size_t i = 0; i < number_of_prime; i++) {
      if (__builtin_expect(n % primes[i] == 0, 0)) {
        result.emplace_back(primes[i], factor(n, primes[i]));
      }
    }
    if (n > 1) {
      result.emplace_back(n, 1);
    }
    return result;
  }

private:
  __attribute__((noinline)) static int factor(uint64_t &n, uint64_t p) {
    int result{0};
    do {
      result++;
      n /= p;
    } while (n % p == 0);
    return result;
  }

  static constexpr size_t get_sqrt_n() {
    size_t p = 1;
    while (p * p <= N) {
      p++;
    }
    return p;
  }

  static constexpr size_t sqrt_n = get_sqrt_n();

  using IsComp = std::array<bool, sqrt_n>;

  static constexpr IsComp get_is_comp() {
    IsComp is_comp{};
    for (int p = 2; p * p < sqrt_n; p++) {
      if (!is_comp[p]) {
        for (int i = p * p; i < sqrt_n; i += p) {
          is_comp[i] = true;
        }
      }
    }
    return is_comp;
  }

  static constexpr IsComp is_comp = get_is_comp();

  static constexpr size_t count_prime() {
    size_t count{0};
    for (int p = 2; p < sqrt_n; p++) {
      count += !is_comp[p];
    }
    return count;
  }

  static constexpr size_t number_of_prime = count_prime();

  using Primes = std::array<int, number_of_prime>;

  static constexpr Primes get_primes() {
    Primes primes{};
    int count{0};
    for (int p = 2; p < sqrt_n; p++) {
      if (!is_comp[p]) {
        primes[count++] = p;
      }
    }
    return primes;
  }

  static constexpr Primes primes = get_primes();
};

TEST_CASE("asm_factor") {
  using Factor = AsmFactor<1'000'000'000>;
  Factor factor{};
  std::cerr << factor(12345678) << std::endl;
}
