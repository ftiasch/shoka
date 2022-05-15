#include "ntt.h"

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
    int m = min_power_of_two(std::max(c.size(), a.size()));
    Poly<NTT>::assert_max_n(m);
    std::vector<std::vector<ModT>> q(m << 1), p(m << 1);
    for (int i = 0; i < m; ++i) {
      q[m + i] = {ModT{1}, i < a.size() ? -a[i] : ModT{0}};
    }
    for (int i = m; i-- > 1;) {
      q[i] = Poly<NTT>::multiply(q[i << 1], q[i << 1 | 1]);
    }
    std::vector<ModT> inv_q(m + 1);
    Poly<NTT>::inverse(m, inv_q.data(), q[1].data());
    auto reshaped_c = c;
    reshaped_c.resize(m << 1);
    p[1] = mul_t(m, inv_q, reshaped_c);
    for (int i = 1; i < m; ++i) {
      int l = p[i].size() >> 1;
      p[i << 1] = mul_t(l, q[i << 1 | 1], p[i]);
      p[i << 1 | 1] = mul_t(l, q[i << 1], p[i]);
    }
    std::vector<ModT> result(a.size());
    for (int i = 0; i < a.size(); ++i) {
      result[i] = p[m + i][0];
    }
    return result;
  }

private:
  std::vector<ModT> mul_t(int n, const std::vector<ModT> &a,
                          const std::vector<ModT> &c) {
    if (a.size() != n + 1) {
      throw std::logic_error("");
    }
    if (c.size() != (n << 1)) {
      throw std::logic_error("");
    }
    ModT *const b0 = Poly<NTT>::buffer[0].data();
    ModT *const b1 = Poly<NTT>::buffer[1].data();
    Poly<NTT>::copy_and_fill0(n << 1, b0, n + 1, a.data());
    std::reverse(b0, b0 + (n + 1));
    Poly<NTT>::copy_and_fill0(n << 1, b1, n << 1, c.data());
    NTT::dif(n << 1, b0);
    NTT::dif(n << 1, b1);
    Poly<NTT>::dot_product_and_dit(n << 1, ModT(n << 1).inverse(), b0, b0, b1);
    std::vector<ModT> b(n);
    std::copy(b0 + n, b0 + (n << 1), b.data());
    return b;
  }
};

} // namespace ntt
