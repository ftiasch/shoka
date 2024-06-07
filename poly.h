#pragma once

#include "ntt.h"
#include "singleton.h"
#include "snippets/min_pow_of_two.h"

#include <cstdlib>
#include <vector>

template <typename Mod_> struct PolyT : public std::vector<Mod_> {
  using Mod = Mod_;
  using Vector = std::vector<Mod>;

  static void assert_power_of_two(int n) { Ntt::assert_power_of_two(n); }
  static void reserve(int n) { return ntt().reserve(n); }
  template <int i> static Mod *raw_buffer() {
    return ntt().template raw_buffer<i>();
  }
  static Mod power_of_two_inv(int n) { return ntt().power_of_two_inv(n); }
  static void dif(int n, Mod *a) { ntt().dif(n, a); }
  static void dit(int n, Mod *a) { ntt().dit(n, a); }

  static void copy_and_fill0(int n, Mod *dst, int m, const Mod *src) {
    m = std::min(n, m);
    std::copy(src, src + m, dst);
    std::fill(dst + m, dst + n, Mod{0});
  }

  static void copy_and_fill0(int n, Mod *dst, const std::vector<Mod> &src) {
    copy_and_fill0(n, dst, src.size(), src.data());
  }

  static void dot_product_and_dit(int n, Mod *out, const Mod *a, const Mod *b) {
    auto inv_n = power_of_two_inv(n);
    for (int i = 0; i < n; ++i) {
      out[i] = inv_n * a[i] * b[i];
    }
    ntt().dit(n, out);
  }

  PolyT() : std::vector<Mod>{Mod{}} {}
  explicit PolyT(size_t size) : std::vector<Mod>(size) {}
  explicit PolyT(Vector &&v) : std::vector<Mod>{std::move(v)} {}
  explicit PolyT(const Vector &v) : std::vector<Mod>{v} {}
  explicit PolyT(const std::initializer_list<Mod> &v) : std::vector<Mod>{v} {}

  const Vector &vector() const { return *this; }

  int deg() const { return static_cast<int>(std::vector<Mod>::size()) - 1; }

  bool operator==(const PolyT &o) const { return vector() == o.vector(); }
  bool operator!=(const PolyT &o) const { return !(*this == o); }

  PolyT operator+(const PolyT &o) const {
    auto copy = *this;
    copy += o;
    return copy;
  }

  PolyT &operator+=(const PolyT &o) {
    if (deg() < o.deg()) {
      this->resize(o.size());
    }
    for (int i = 0; i <= o.deg(); i++) {
      (*this)[i] += o[i];
    }
    return *this;
  }

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

  PolyT &operator-=(const PolyT &o) { return *this = *this - o; }

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
    int n = min_pow_of_two(deg_plus_1);
    reserve(n);
    Mod *b0 = raw_buffer<0>();
    Mod *b1 = raw_buffer<1>();
    copy_and_fill0(n, b0, *this);
    dif(n, b0);
    copy_and_fill0(n, b1, o);
    dif(n, b1);
    dot_product_and_dit(n, b0, b0, b1);
    return PolyT(std::vector<Mod>{b0, b0 + deg_plus_1});
  }

  PolyT &operator*=(const PolyT &o) { return *this = *this * o; }

private:
  using Ntt = NttT<Mod>;

  static Ntt &ntt() { return singleton<Ntt>(); }
};
