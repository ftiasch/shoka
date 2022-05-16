#include "ntt.h"

#include <chrono>
#include <vector>

namespace ntt {

template <typename NTT> struct MultiEval : public Poly<NTT> {
private:
  using ModT = typename Poly<NTT>::ModT;

public:
  MultiEval(int max_n_) : Poly<NTT>(max_n_ << 1) {}

  // input:  f(z) = sum c[i] * z^i
  // output: f(a_0) f(a_1) ...
  std::vector<ModT> eval(const std::vector<ModT> &c,
                         const std::vector<ModT> &a) {
    int m = min_power_of_two(std::max<size_t>({c.size(), a.size(), 2}));
    Poly<NTT>::assert_max_n(m);
    int log_m = (__builtin_ctz(m)) + 1;
    std::vector<std::vector<ModT>> dif_rev_q(log_m, std::vector<ModT>(m << 1));
    for (int i = 0; i < m; ++i) {
      dif_rev_q[0][i << 1] = i < a.size() ? -a[i] : ModT(0);
      dif_rev_q[0][i << 1 | 1] = ModT{1};
      NTT::dif(2, dif_rev_q[0].data() + (i << 1));
    }
    for (int l = 1; l < log_m; ++l) {
      ModT inv_n(ModT(1 << l).inverse());
      ModT G(NTT::get_primitive_root());
      ModT zeta(G.power(ModT::MOD - 1 - (ModT::MOD >> (l + 1))));
      for (int s = 0; s < (m << 1); s += (2 << l)) {
        if (l + 1 < log_m) {
          for (int i = s; i < s + (1 << l); ++i) {
            dif_rev_q[l][i] = dif_rev_q[l - 1][i] * dif_rev_q[l - 1][i + (1 << l)];
            dif_rev_q[l][i + (1 << l)] = inv_n * dif_rev_q[l][i];
          }
          NTT::dit(1 << l, dif_rev_q[l].data() + s + (1 << l));
          dif_rev_q[l][s + (1 << l)] -= ModT{2};
          ModT zeta_tmp{1};
          for (int i = s + (1 << l); i < s + (2 << l); ++i) {
            dif_rev_q[l][i] *= zeta_tmp;
            zeta_tmp *= zeta;
          }
          NTT::dif(1 << l, dif_rev_q[l].data() + s + (1 << l));
        } else {
          Poly<NTT>::dot_product_and_dit(1 << l, inv_n,
                                       dif_rev_q[l].data() + s,
                                       dif_rev_q[l - 1].data() + s,
                                       dif_rev_q[l - 1].data() + s + (1 << l));
          dif_rev_q[l][s] -= ModT{1};
          dif_rev_q[l][s + (1 << l)] = ModT{1};
        }
      }
    }
    auto &q1 = dif_rev_q[log_m - 1];
    std::reverse(q1.data(), q1.data() + (m + 1));
    ModT *const dif_rev_inv_q1 = Poly<NTT>::buffer[2].data();
    Poly<NTT>::inverse(m, dif_rev_inv_q1, q1.data());
    // mul_t(m, inv_q1, c)
    std::fill(dif_rev_inv_q1 + m, dif_rev_inv_q1 + (m << 1), ModT{0});
    std::reverse(dif_rev_inv_q1, dif_rev_inv_q1 + (m + 1));
    NTT::dif(m << 1, dif_rev_inv_q1);
    ModT *const dif_c = Poly<NTT>::buffer[3].data();
    Poly<NTT>::copy_and_fill0(m << 1, dif_c, c.size(), c.data());
    NTT::dif(m << 1, dif_c);
    ModT *pnow = Poly<NTT>::buffer[0].data();
    ModT *ppre = Poly<NTT>::buffer[1].data();
    mul_t(m << 1, pnow, dif_rev_inv_q1, dif_c);
    for (int l = log_m; l-- > 1;) {
      std::swap(pnow, ppre);
      for (int s = 0, s2 = 0; s < m; s += (1 << l), s2 += (2 << l)) {
        NTT::dif(1 << l, ppre + s);
        mul_t(1 << l, pnow + s, dif_rev_q[l - 1].data() + s2 + (1 << l),
              ppre + s);
        mul_t(1 << l, pnow + s + (1 << (l - 1)), dif_rev_q[l - 1].data() + s2,
              ppre + s);
      }
    }
    return std::vector<ModT>(pnow, pnow + a.size());
  }

private:
  void mul_t(int n, ModT *out, ModT *dif_rev_a, ModT *dif_c) {
    ModT *const b = Poly<NTT>::buffer[2].data();
    Poly<NTT>::dot_product_and_dit(n, ModT(n).inverse(), b, dif_rev_a, dif_c);
    std::copy(b + (n >> 1), b + n, out);
  }
};

} // namespace ntt
