#pragma once

#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <vector>

template <typename Factory>
struct PolyT : public std::vector<typename Factory::Mod> {
  using Mod = typename Factory::Mod;
  using NTT = typename Factory::NTT;

  PolyT() = default;

  template <typename... Args>
  PolyT(std::shared_ptr<Factory> &&factory_, Args &&...args)
      : std::vector<Mod>(std::forward<Args>(args)...),
        factory(std::move(factory_)) {}

  int deg() const { return static_cast<int>(std::vector<Mod>::size()) - 1; }

  PolyT operator+(const PolyT &o) const {
    if (deg() < o.deg()) {
      return o + *this;
    }
    PolyT r = factory->make(*this);
    for (int i = 0; i <= o.deg(); ++i) {
      r[i] += o[i];
    }
    return r;
  }

  PolyT &operator+=(PolyT &o) { return *this = *this += o; }

  PolyT operator-(const PolyT &o) const {
    int max_deg = std::max(deg(), o.deg());
    PolyT r = factory->make(max_deg + 1);
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
      std::vector<Mod> result(deg_plus_1);
      for (int i = 0; i <= deg(); ++i) {
        for (int j = 0; j <= o.deg(); ++j) {
          result[i + j] += (*this)[i] * o[j];
        }
      }
      return factory->make(std::move(result));
    }

    int n = Factory::min_power_of_two(deg_plus_1);
    factory->reserve(n);
    Mod *b0 = factory->template raw_buffer<0>();
    Mod *b1 = factory->template raw_buffer<1>();
    Factory::copy_and_fill0(n, b0, *this);
    NTT::dif(n, b0);
    Factory::copy_and_fill0(n, b1, o);
    NTT::dif(n, b1);
    Factory::dot_product_and_dit(n, Mod(n).inverse(), b0, b0, b1);
    return factory->make(std::vector<Mod>(b0, b0 + deg_plus_1));
  }

  PolyT &operator*=(const PolyT &o) { return *this = *this * o; }

  std::shared_ptr<Factory> factory;
};

template <typename NTT_>
struct PolyFactory : std::enable_shared_from_this<PolyFactory<NTT_>> {
  static const size_t NUMBER_OF_BUFFER = 5;

  using NTT = NTT_;
  using Mod = typename NTT::Mod;
  using Poly = PolyT<PolyFactory>;

  static std::shared_ptr<PolyFactory> create() {
    return std::shared_ptr<PolyFactory>(new PolyFactory());
  }

  template <typename... Args> Poly make(Args &&...args) {
    PolyT p{std::enable_shared_from_this<PolyFactory>::shared_from_this(),
            std::forward<Args>(args)...};
    return p;
  }

  static void assert_power_of_two(int n) {
    if (n & (n - 1)) {
      throw std::invalid_argument(std::to_string(n) + " is not a power of two");
    }
  }

  static int min_power_of_two(int n) {
    return n == 1 ? 1 : 1 << (32 - __builtin_clz(n - 1));
  }

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

protected:
  PolyFactory() = default;

private:
  std::array<std::vector<Mod>, NUMBER_OF_BUFFER> buffer;
};
