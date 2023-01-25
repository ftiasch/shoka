#include <cmath>
#include <iostream>

template <typename PT = int, typename P2T = long long> struct PointT {
  PointT &operator+=(const PointT &o) {
    x += o.x;
    y += o.y;
    return *this;
  }

  PointT operator+(const PointT &o) const {
    PointT copy = *this;
    return copy += o;
  }

  PointT &operator-=(const PointT &o) {
    x -= o.x;
    y -= o.y;
    return *this;
  }

  PointT operator-() const { return {-x, -y}; }

  PointT operator-(const PointT &o) const {
    PointT copy = *this;
    return copy -= o;
  }

  PointT to(const PointT &o) const { return o - *this; }

  P2T dot(const PointT &o) const {
    return static_cast<P2T>(x) * o.x + static_cast<P2T>(y) * o.y;
  }

  P2T det(const PointT &o) const {
    return static_cast<P2T>(x) * o.y - static_cast<P2T>(y) * o.x;
  }

  double norm() const { return std::hypot<double>(y, x); }

  P2T norm2() const { return dot(*this); }

  PT x, y;
};

template <typename PT, typename P2T>
std::ostream &operator<<(std::ostream &out, const PointT<PT, P2T> &p) {
  return out << "(" << p.x << ", " << p.y << ")";
}
