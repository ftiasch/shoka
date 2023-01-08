#include <vector>

template <typename Mod> struct Binom {
  explicit Binom(int n) : fact(n), inv_fact(n) {
    fact[0] = inv_fact[0] = inv_fact[1] = Mod{1};
    for (int i = 2; i < n; ++i) {
      inv_fact[i] = -Mod{Mod::mod() / i} * inv_fact[Mod::mod() % i];
    }
    for (int i = 1; i < n; ++i) {
      fact[i] = Mod{i} * fact[i - 1];
      inv_fact[i] *= inv_fact[i - 1];
    }
  }

  Mod operator()(int n, int k) const {
    return k < 0 || k > n ? Mod{0} : fact[n] * inv_fact[n - k] * inv_fact[k];
  }

  std::vector<Mod> fact, inv_fact;
};
