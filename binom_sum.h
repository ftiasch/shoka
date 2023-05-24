#include "dyn_inv_table.h"

#include <cassert>
#include <vector>

template <typename Mod> struct BinomialSum {
  // c0 * x^n mod (x - 1)^{k + 1}
  static std::vector<Mod> monomial_mod(Mod n, int k, Mod c0 = Mod{1}) {
    std::vector<Mod> result(k + 1);
    if (n.get() <= k) {
      result[n.get()] = c0;
    } else {
      result[0] = Mod{1};
      for (int i = 0; i < k; i++) {
        result[i + 1] = result[i] * (n - Mod{i}) * inv<Mod>(i + 1);
      }
      Mod c{c0};
      for (int i = k; i >= 0; i--) {
        result[i] *= c;
        c = -c * (n - Mod{i}) * inv<Mod>(k - i + 1);
      }
    }
    return result;
  }

  // Q * F(z) = P
  // Computes F(z) mod (z - 1)^{k + 1}
  // where P is given in sparse form, i.e. (i, c) stands for c * z^i
  static std::vector<Mod>
  rational_gf_mod(const std::vector<Mod> &Q,
                  const std::vector<std::pair<Mod, Mod>> &P, int k) {
    auto d = static_cast<int>(Q.size()) - 1;
    // Q(t + 1)
    std::vector<Mod> Q_in_t(d + 1);
    for (int i = 0; i <= d; i++) {
      Mod c = Q[i];
      for (int j = 0; j <= i; j++) {
        // Q_in_t[j] += Q[i] * binom(i, j);
        Q_in_t[j] += c;
        c *= Mod{i - j} * inv<Mod>(j + 1);
      }
    }
    assert(Q_in_t[0].get());
    // P(t + 1) mod t^{k + 1}
    std::vector<Mod> F_in_t(k + 1);
    for (auto &&[i, pi] : P) {
      Mod c = pi;
      for (int j = 0; j <= k && c.get(); j++) {
        F_in_t[j] += c;
        c *= (i - Mod{j}) * inv<Mod>(j + 1);
      }
    }
    {
      auto inv_Q0 = Q_in_t[0].inv();
      for (int i = 0; i <= k; i++) {
        Mod t{F_in_t[i]};
        for (int j = 1; j <= d && j <= i; j++) {
          t -= Q_in_t[j] * F_in_t[i - j];
        }
        F_in_t[i] = inv_Q0 * t;
      }
    }
    std::vector<Mod> F_in_u(k + 1);
    for (auto &&[i, pi] : P) {
      if (i.get() <= k) {
        F_in_u[i.get()] += pi;
      } else {
        auto c = monomial_mod(i, k, pi);
        for (int j = 0; j <= k; j++) {
          F_in_u[j] += c[j];
        }
      }
    }
    // offset terms
    for (int i = 1; i <= d; i++) {
      // t^{k + i}
      Mod c{0};
      for (int j = i; j <= d; j++) {
        c += Q_in_t[j] * F_in_t[k + i - j];
      }
      // (u - 1)^{k + i}
      if ((k + i) & 1) {
        c = -c;
      }
      for (int j = 0; j <= k && c.get(); j++) {
        F_in_u[j] += c;
        c = -c * Mod{k + i - j} * inv<Mod>(j + 1);
      }
    }
    {
      auto inv_Q0 = Q[0].inv();
      for (int i = 0; i <= k; i++) {
        Mod t{F_in_u[i]};
        for (int j = 1; j <= d && j <= i; j++) {
          t -= Q[j] * F_in_u[i - j];
        }
        F_in_u[i] = inv_Q0 * t;
      }
    }
    return F_in_u;
  }
};
