#include "poly_log.h"

template <typename Poly> struct PolyExp : public PolyOp<Poly, PolyExp> {
  using Base = PolyOp<Poly, PolyExp>;
  SHOKA_HELPER_USING_POLY_OP;

  Poly operator()(const Poly &f) { return Base::template single<4>(f); }

  void _(int n, Mod *out, const Mod *f) {
    if (f[0] != Mod{0}) {
      throw std::invalid_argument("[x^0] f != 0");
    }
    if (n == 1) {
      out[0] = Mod{1};
    } else {
      Poly::assert_power_of_two(n);
      Poly::reserve(n);
      const auto b0 = Poly::template raw_buffer<0>();
      const auto b1 = Poly::template raw_buffer<1>();
      const auto b2 = Poly::template raw_buffer<2>();
      const auto b3 = Poly::template raw_buffer<3>();
      out[0] = b1[0] = b1[1] = b2[0] = Mod{1};
      for (int m = 1; m < (n >> 1); m <<= 1) {
        for (int i = 0; i < m; ++i) {
          b0[i] = Mod{i} * f[i];
        }
        Poly::dif(m, b0);
        Poly::dot_product_and_dit(m, b0, b0, b1);
        for (int i = 0; i < m; ++i) {
          b0[i] -= Mod{i} * out[i];
        }
        std::fill(b0 + m, b0 + (m << 1), Mod{0});
        Poly::dif(m << 1, b0);
        Poly::copy_and_fill0(m << 1, b3, m, b2);
        Poly::dif(m << 1, b3);
        Poly::dot_product_and_dit(m << 1, b0, b0, b3);
        for (int i = 0; i < m; ++i) {
          b0[i] = b0[i] * log.cached_inv(m + i) + f[m + i];
        }
        std::fill(b0 + m, b0 + (m << 1), Mod{0});
        Poly::dif(m << 1, b0);
        Poly::dot_product_and_dit(m << 1, b0, b0, b1);
        std::copy(b0, b0 + m, out + m);
        Poly::copy_and_fill0(m << 2, b1, m << 1, out);
        Poly::dif(m << 2, b1);
        Poly::dot_product_and_dit(m << 1, b0, b1, b3);
        std::fill(b0, b0 + m, Mod{0});
        Poly::dif(m << 1, b0);
        Poly::dot_product_and_dit(m << 1, b0, b0, b3);
        for (int i = m; i < m << 1; ++i) {
          b2[i] = Mod{0} - b0[i];
        }
      }
      int m = n >> 1;
      for (int i = 0; i < m; ++i) {
        b0[i] = Mod{i} * f[i];
      }
      Poly::dif(m, b0);
      Poly::dot_product_and_dit(m, b0, b0, b1);
      for (int i = 0; i < m; ++i) {
        b0[i] -= Mod{i} * out[i];
      }
      Poly::copy_and_fill0(m, b0 + m, m >> 1, b0 + (m >> 1));
      std::fill(b0 + (m >> 1), b0 + m, Mod{0});
      Poly::dif(m, b0);
      Poly::dif(m, b0 + m);
      Poly::copy_and_fill0(m, b3 + m, m >> 1, b2 + (m >> 1));
      Poly::dif(m, b3 + m);
      auto inv_m = Poly::power_of_two_inv(m);
      for (int i = 0; i < m; ++i) {
        b0[m + i] = inv_m * (b0[i] * b3[m + i] + b0[m + i] * b3[i]);
      }
      Poly::dot_product_and_dit(m, b0, b0, b3);
      Poly::dit(m, b0 + m);
      for (int i = 0; i < m >> 1; ++i) {
        b0[(m >> 1) + i] += b0[m + i];
      }
      for (int i = 0; i < m; ++i) {
        b0[i] = b0[i] * log.cached_inv(m + i) + f[m + i];
      }
      std::fill(b0 + m, b0 + (m << 1), Mod{0});
      Poly::dif(m << 1, b0);
      Poly::dot_product_and_dit(m << 1, b0, b0, b1);
      std::copy(b0, b0 + m, out + m);
    }
  }

private:
  PolyLog<Poly> log;
};
