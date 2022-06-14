#include "ntt_util.h"

#include <array>
#include <vector>

namespace ntt {

/*
 * Operation in Finite Polynomial Ring
 * e.g. 1 / f(z) is not finite and should be computed as 1 / f(z) mod z^n
 */
template <typename NTT> struct FinitePolyFactoryT {
  using Mod = typename NTT::Mod;
  using Factory = FinitePolyFactoryT<NTT>;

  struct Poly : public std::vector<Mod> {
    Poly(Factory *factory_, const std::vector<Mod> &coefficient)
        : factory{factory_}, std::vector<Mod>{coefficient} {}

    Poly(Factory *factory_, std::vector<Mod> &&coefficient)
        : factory{factory_}, std::vector<Mod>{std::move(coefficient)} {}

    int deg() const { return static_cast<int>(std::vector<Mod>::size()) - 1; }

    Poly operator+(const Poly &o) const {
      if (deg() < o.deg()) {
        return o + *this;
      }
      Poly r = factory->make_poly(*this);
      for (int i = 0; i <= o.deg(); ++i) {
        r[i] += o[i];
      }
      return r;
    }

    Poly &operator+=(const Poly &o) { return *this = *this += o; }

    Poly operator-(const Poly &o) const {
      int max_deg = std::max(deg(), o.deg());
      Poly r = factory->make_poly(std::vector<Mod>(max_deg + 1));
      int min_deg = std::min(deg(), o.deg());
      for (int i = 0; i <= min_deg; ++i) {
        r[i] = (*this)[i] - o[i];
      }
      for (int i = min_deg + 1; i <= deg(); ++i) {
        r[i] = (*this)[i];
      }
      for (int i = min_deg + 1; i <= o.deg(); ++i) {
        r[i] = -o[i];
      }
      return r;
    }

    Poly &operator-=(const Poly &o) { return *this = *this -= o; }

    Poly operator*(const Poly &o) const {
      Mod *b0 = factory->raw_buffer<0>();
      Mod *b1 = factory->raw_buffer<1>();

      int deg_plus_1 = deg() + o.deg() + 1;
      int n = min_power_of_two(deg_plus_1);
      factory->assert_max_n(n);

      copy_and_fill0(n, b0, *this);
      NTT::dif(n, b0);
      copy_and_fill0(n, b1, o);
      NTT::dif(n, b1);
      dot_product_and_dit(n, Mod(n).inverse(), b0, b0, b1);
      return factory->make_poly(std::vector<Mod>(b0, b0 + deg_plus_1));
    }

    Poly &operator*=(const Poly &o) { return *this = *this * o; }

    Factory *factory;
  };

  friend struct Poly;

  FinitePolyFactoryT(int max_deg)
      : max_n{min_power_of_two(max_deg + 1)}, buffer{std::vector<Mod>(max_n),
                                                     std::vector<Mod>(max_n)} {}

  Poly make_poly(const std::vector<Mod> &coefficient) {
    return Poly{this, coefficient};
  }

  Poly make_poly(std::vector<Mod> &&coefficient) {
    return Poly{this, std::move(coefficient)};
  }

  static void copy_and_fill0(int n, Mod *dst, int m, const Mod *src) {
    m = std::min(n, m);
    std::copy(src, src + m, dst);
    std::fill(dst + m, dst + n, Mod(0));
  }

  static void copy_and_fill0(int n, Mod *dst, const std::vector<Mod> &src) {
    copy_and_fill0(n, dst, src.size(), src.data());
  }

  static void dot_product_and_dit(int n, Mod inv_n, Mod *out, const Mod *a,
                                  const Mod *b) {
    for (int i = 0; i < n; ++i) {
      out[i] = inv_n * a[i] * b[i];
    }
    NTT::dit(n, out);
  }

  void assert_max_n(int n) const {
    if (n > max_n) {
      throw std::invalid_argument("insufficient buffer");
    }
  }

  template <int index> Mod *raw_buffer() { return buffer[index].data(); }

  int max_n;
  std::array<std::vector<Mod>, 2> buffer;
};

} // namespace ntt
