#include "singleton.h"

#include <ostream>
#include <tuple>

// Fp[z] / (z^2 - alpha)
template <typename Mod> struct GaussianFieldT {
  constexpr GaussianFieldT(Mod x_ = {}, Mod y_ = {}) : x{x_}, y{y_} {}

  static Mod &a() { return singleton<Store>().alpha; }

  static void set_alpha(Mod a_) { a() = a_; }

  static constexpr GaussianFieldT mul_id() { return {Mod::mul_id(), Mod{}}; }

  constexpr GaussianFieldT &operator+=(const GaussianFieldT &other) {
    x += other.x;
    y += other.y;
    return *this;
  }

  constexpr GaussianFieldT &operator-=(const GaussianFieldT &other) {
    x -= other.x;
    y -= other.y;
    return *this;
  }

  constexpr GaussianFieldT operator*=(const GaussianFieldT &other) {
    std::tie(x, y) = std::pair<Mod, Mod>{x * other.x + a() * y * other.y,
                                         x * other.y + y * other.x};
    return *this;
  }

  constexpr GaussianFieldT operator+(const GaussianFieldT &other) const {
    GaussianFieldT copy = *this;
    return copy += other;
  }

  constexpr GaussianFieldT operator-() const {
    GaussianFieldT copy{};
    copy -= *this;
    return copy;
  }

  constexpr GaussianFieldT operator-(const GaussianFieldT &other) const {
    GaussianFieldT copy = *this;
    return copy -= other;
  }

  constexpr GaussianFieldT operator*(const GaussianFieldT &other) const {
    GaussianFieldT copy = *this;
    return copy *= other;
  }

  constexpr GaussianFieldT inv() const {
    auto d = (x * x - a() * y * y).inv();
    return GaussianFieldT{d * x, -d * y};
  }

  Mod x, y;

private:
  struct Store {
    Mod alpha;
  };
};

namespace std {

template <typename Mod>
std::ostream &operator<<(std::ostream &out, GaussianFieldT<Mod> e) {
  return out << e.x << " + " << e.y << " sqrt{" << GaussianFieldT<Mod>::a()
             << "}";
}

} // namespace std
