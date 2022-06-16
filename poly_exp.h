#include "poly_log.h"

template <typename NTT> struct PolyExp : public PolyOp<PolyExp, NTT> {
  using Base = PolyOp<PolyExp, NTT>;
  using Base::factory;
  using typename Base::Factory;
  using typename Base::Mod;
  using typename Base::Poly;

  PolyExp(std::shared_ptr<Factory> factory_) : Base{factory_}, log{factory} {}

  Poly operator()(const Poly &f) { return Base::template single<4>(f); }

  void _(int n, Mod *out, const Mod *f) {
    if (f[0].get() != 0) {
      throw std::invalid_argument("[x^0] f != 0");
    }
    if (n == 1) {
      out[0] = Mod(1);
    } else {
      Factory::assert_power_of_two(n);
      factory->reserve(n);
      Mod *const b0 = factory->template raw_buffer<0>();
      Mod *const b1 = factory->template raw_buffer<1>();
      Mod *const b2 = factory->template raw_buffer<2>();
      Mod *const b3 = factory->template raw_buffer<3>();
      out[0] = b1[0] = b1[1] = b2[0] = Mod(1);
      Mod inv_m(1);
      for (int m = 1; m < (n >> 1); m <<= 1) {
        const Mod inv_2m = inv_m * Mod(2).inverse();
        for (int i = 0; i < m; ++i) {
          b0[i] = Mod(i) * f[i];
        }
        NTT::dif(m, b0);
        Factory::dot_product_and_dit(m, inv_m, b0, b0, b1);
        for (int i = 0; i < m; ++i) {
          b0[i] -= Mod(i) * out[i];
        }
        std::fill(b0 + m, b0 + (m << 1), Mod(0));
        NTT::dif(m << 1, b0);
        Factory::copy_and_fill0(m << 1, b3, m, b2);
        NTT::dif(m << 1, b3);
        Factory::dot_product_and_dit(m << 1, inv_2m, b0, b0, b3);
        for (int i = 0; i < m; ++i) {
          b0[i] = b0[i] * log.cached_inv(m + i) + f[m + i];
        }
        std::fill(b0 + m, b0 + (m << 1), Mod(0));
        NTT::dif(m << 1, b0);
        Factory::dot_product_and_dit(m << 1, inv_2m, b0, b0, b1);
        std::copy(b0, b0 + m, out + m);
        Factory::copy_and_fill0(m << 2, b1, m << 1, out);
        NTT::dif(m << 2, b1);
        Factory::dot_product_and_dit(m << 1, inv_2m, b0, b1, b3);
        std::fill(b0, b0 + m, Mod(0));
        NTT::dif(m << 1, b0);
        Factory::dot_product_and_dit(m << 1, inv_2m, b0, b0, b3);
        for (int i = m; i < m << 1; ++i) {
          b2[i] = Mod(0) - b0[i];
        }
        inv_m = inv_2m;
      }
      int m = n >> 1;
      for (int i = 0; i < m; ++i) {
        b0[i] = Mod(i) * f[i];
      }
      NTT::dif(m, b0);
      Factory::dot_product_and_dit(m, inv_m, b0, b0, b1);
      for (int i = 0; i < m; ++i) {
        b0[i] -= Mod(i) * out[i];
      }
      Factory::copy_and_fill0(m, b0 + m, m >> 1, b0 + (m >> 1));
      std::fill(b0 + (m >> 1), b0 + m, Mod(0));
      NTT::dif(m, b0);
      NTT::dif(m, b0 + m);
      Factory::copy_and_fill0(m, b3 + m, m >> 1, b2 + (m >> 1));
      NTT::dif(m, b3 + m);
      for (int i = 0; i < m; ++i) {
        b0[m + i] = inv_m * (b0[i] * b3[m + i] + b0[m + i] * b3[i]);
      }
      Factory::dot_product_and_dit(m, inv_m, b0, b0, b3);
      NTT::dit(m, b0 + m);
      for (int i = 0; i < m >> 1; ++i) {
        b0[(m >> 1) + i] += b0[m + i];
      }
      for (int i = 0; i < m; ++i) {
        b0[i] = b0[i] * log.cached_inv(m + i) + f[m + i];
      }
      std::fill(b0 + m, b0 + (m << 1), Mod(0));
      NTT::dif(m << 1, b0);
      Mod inv_2m = inv_m * Mod(2).inverse();
      Factory::dot_product_and_dit(m << 1, inv_2m, b0, b0, b1);
      std::copy(b0, b0 + m, out + m);
    }
  }

  PolyLog<NTT> log;
};
