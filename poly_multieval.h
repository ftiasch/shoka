#include "binpow.h"
#include "poly_inv.h"
#include "primitive_root.h"
#include "snippets/min_pow_of_two.h"

#include <algorithm>
#include <vector>

template <typename Poly>
struct PolyMultiEval : public PolyOp<Poly, PolyMultiEval> {
  using Base = PolyOp<Poly, PolyMultiEval>;
  SHOKA_HELPER_USING_POLY_OP;

  // input:  f(z) = sum c[i] * z^i
  // output: f(a_0) f(a_1) ...
  std::vector<Mod> operator()(const std::vector<Mod> &c,
                              const std::vector<Mod> &a) {
    int m = min_pow_of_two(std::max<size_t>({c.size(), a.size(), 2}));
    Poly::reserve(m << 1);
    int log_m = (__builtin_ctz(m)) + 1;
    std::vector<std::vector<Mod>> dif_rev_q(log_m, std::vector<Mod>(m << 1));
    for (int i = 0; i < m; ++i) {
      dif_rev_q[0][i << 1] = i < a.size() ? -a[i] : Mod{0};
      dif_rev_q[0][i << 1 | 1] = Mod{1};
      Poly::dif(2, dif_rev_q[0].data() + (i << 1));
    }
    for (int l = 1; l < log_m; ++l) {
      Mod inv_n{Poly::power_of_two_inv(1 << l)},
          G{FiniteField<Mod>::primitive_root()},
          zeta{binpow(G, Mod::mod() - 1 - (Mod::mod() >> (l + 1)))};
      for (int s = 0; s < (m << 1); s += (2 << l)) {
        if (l + 1 < log_m) {
          for (int i = s; i < s + (1 << l); ++i) {
            dif_rev_q[l][i] =
                dif_rev_q[l - 1][i] * dif_rev_q[l - 1][i + (1 << l)];
            dif_rev_q[l][i + (1 << l)] = inv_n * dif_rev_q[l][i];
          }
          Poly::dit(1 << l, dif_rev_q[l].data() + s + (1 << l));
          dif_rev_q[l][s + (1 << l)] -= Mod{2};
          Mod zeta_tmp{1};
          for (int i = s + (1 << l); i < s + (2 << l); ++i) {
            dif_rev_q[l][i] *= zeta_tmp;
            zeta_tmp *= zeta;
          }
          Poly::dif(1 << l, dif_rev_q[l].data() + s + (1 << l));
        } else {
          Poly::dot_product_and_dit(1 << l, dif_rev_q[l].data() + s,
                                    dif_rev_q[l - 1].data() + s,
                                    dif_rev_q[l - 1].data() + s + (1 << l));
          dif_rev_q[l][s] -= Mod{1};
          dif_rev_q[l][s + (1 << l)] = Mod{1};
        }
      }
    }
    auto &q1 = dif_rev_q[log_m - 1];
    std::reverse(q1.data(), q1.data() + (m + 1));
    const auto dif_rev_inv_q1 = Poly::template raw_buffer<2>();
    inv._(m, dif_rev_inv_q1, q1.data());
    // mul_t(m, inv_q1, c)
    std::fill(dif_rev_inv_q1 + m, dif_rev_inv_q1 + (m << 1), Mod{0});
    std::reverse(dif_rev_inv_q1, dif_rev_inv_q1 + (m + 1));
    Poly::dif(m << 1, dif_rev_inv_q1);
    const auto dif_c = Poly::template raw_buffer<3>();
    Poly::copy_and_fill0(m << 1, dif_c, c.size(), c.data());
    Poly::dif(m << 1, dif_c);
    auto pnow = Poly::template raw_buffer<0>();
    auto ppre = Poly::template raw_buffer<1>();
    mul_t(m << 1, pnow, dif_rev_inv_q1, dif_c);
    for (int l = log_m; l-- > 1;) {
      std::swap(pnow, ppre);
      for (int s = 0, s2 = 0; s < m; s += (1 << l), s2 += (2 << l)) {
        Poly::dif(1 << l, ppre + s);
        mul_t(1 << l, pnow + s, dif_rev_q[l - 1].data() + s2 + (1 << l),
              ppre + s);
        mul_t(1 << l, pnow + s + (1 << (l - 1)), dif_rev_q[l - 1].data() + s2,
              ppre + s);
      }
    }
    return std::vector<Mod>(pnow, pnow + a.size());
  }

private:
  void mul_t(int n, Mod *out, Mod *dif_rev_a, Mod *dif_c) {
    const auto b = Poly::template raw_buffer<2>();
    Poly::dot_product_and_dit(n, b, dif_rev_a, dif_c);
    std::copy(b + (n >> 1), b + n, out);
  }

  PolyInv<Poly> inv;
};
