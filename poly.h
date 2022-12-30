#pragma once

#include "ntt.h"
#include "singleton.h"

#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <vector>

template <typename Mod_> struct PolyT : public std::vector<Mod_> {
  using Mod = Mod_;
  using Ntt = NttT<Mod>;
  using Vector = std::vector<Mod>;

  using Vector::vector;

  explicit PolyT(const Vector &v) : std::vector<Mod>{v} {}

  int deg() const { return static_cast<int>(std::vector<Mod>::size()) - 1; }

  const Vector &vector() const { return *this; }

  PolyT operator+(const PolyT &o) const {
    if (deg() < o.deg()) {
      return o + *this;
    }
    auto r = *this;
    for (int i = 0; i <= o.deg(); ++i) {
      r[i] += o[i];
    }
    return r;
  }

  PolyT &operator+=(PolyT &o) { return *this = *this += o; }

  PolyT operator-(const PolyT &o) const {
    int max_deg = std::max(deg(), o.deg());
    PolyT r(max_deg + 1);
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

  PolyT &operator-=(PolyT &o) { return *this = *this -= o; }

  PolyT operator*(const PolyT &o) const {
    int deg_plus_1 = deg() + o.deg() + 1;
    if (deg_plus_1 <= 16) {
      PolyT result(deg_plus_1);
      for (int i = 0; i <= deg(); ++i) {
        for (int j = 0; j <= o.deg(); ++j) {
          result[i + j] += (*this)[i] * o[j];
        }
      }
      return result;
    }

    int n = Ntt::min_power_of_two(deg_plus_1);
    factory().reserve(n);
    Mod *b0 = factory().template raw_buffer<0>();
    Mod *b1 = factory().template raw_buffer<1>();
    Factory::copy_and_fill0(n, b0, *this);
    Ntt::dif(n, b0);
    Factory::copy_and_fill0(n, b1, o);
    Ntt::dif(n, b1);
    Factory::dot_product_and_dit(n, Mod{n}.inv(), b0, b0, b1);
    return PolyT(b0, b0 + deg_plus_1);
  }

  PolyT &operator*=(const PolyT &o) { return *this = *this * o; }

  struct Factory {
    static const size_t NUMBER_OF_BUFFER = 5;

    void reserve(int n) {
      if (buffer[0].size() < n) {
        for (int i = 0; i < NUMBER_OF_BUFFER; ++i) {
          buffer[i].resize(n);
        }
      }
    }

    template <int index> Mod *raw_buffer() {
      static_assert(0 <= index && index < NUMBER_OF_BUFFER);
      return buffer[index].data();
    }

    static void copy_and_fill0(int n, Mod *dst, int m, const Mod *src) {
      m = std::min(n, m);
      std::copy(src, src + m, dst);
      std::fill(dst + m, dst + n, Mod{0});
    }

    static void copy_and_fill0(int n, Mod *dst, const std::vector<Mod> &src) {
      copy_and_fill0(n, dst, src.size(), src.data());
    }

    static void dot_product_and_dit(int n, Mod inv_n, Mod *out, const Mod *a,
                                    const Mod *b) {
      for (int i = 0; i < n; ++i) {
        out[i] = inv_n * a[i] * b[i];
      }
      Ntt::dit(n, out);
    }

  private:
    std::array<std::vector<Mod>, NUMBER_OF_BUFFER> buffer;
  };

  static Factory &factory() { return Singleton<Factory>::instance(); }
};
