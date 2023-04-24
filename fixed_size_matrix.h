#pragma once

#include <array>
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <vector>

template <typename T, int N>
struct FixedSizeMatrixT : std::array<std::array<T, N>, N> {
  using Row = std::array<T, N>;

  using std::array<Row, N>::array;

  explicit FixedSizeMatrixT(std::initializer_list<Row> rows)
      : std::array<Row, N>{
            reinterpret_cast<const std::array<Row, N> &>(*rows.begin())} {}

  static FixedSizeMatrixT mul_id() {
    FixedSizeMatrixT e;
    for (int i = 0; i < N; ++i) {
      e[i][i] = T::mul_id();
    }
    return e;
  }

  FixedSizeMatrixT &operator+=(const FixedSizeMatrixT &o) {
    for (int i = 0; i < N; ++i) {
      for (int j = 0; j < N; ++j) {
        (*this)[i][j] += o[i][j];
      }
    }
    return *this;
  }

  FixedSizeMatrixT operator+(const FixedSizeMatrixT &o) const {
    return FixedSizeMatrixT(*this) += o;
  }

  FixedSizeMatrixT &operator-=(const FixedSizeMatrixT &o) {
    for (int i = 0; i < N; ++i) {
      for (int j = 0; j < N; ++j) {
        (*this)[i][j] -= o[i][j];
      }
    }
    return *this;
  }

  FixedSizeMatrixT operator-(const FixedSizeMatrixT &o) const {
    return FixedSizeMatrixT(*this) -= o;
  }

  FixedSizeMatrixT operator*(const FixedSizeMatrixT &o) const {
    static T buffer[N];
    FixedSizeMatrixT result;
    for (int j = 0; j < N; ++j) {
      for (int i = 0; i < N; ++i) {
        buffer[i] = o[i][j];
      }
      for (int i = 0; i < N; ++i) {
        for (int k = 0; k < N; ++k) {
          result[i][j] += (*this)[i][k] * buffer[k];
        }
      }
    }
    return result;
  }

  FixedSizeMatrixT &operator*=(const FixedSizeMatrixT &o) {
    return *this = *this * o;
  }
};

template <typename T, int N>
auto operator*(const typename FixedSizeMatrixT<T, N>::Row &r,
               const FixedSizeMatrixT<T, N> &o) {
  typename FixedSizeMatrixT<T, N>::Row result;
  for (int k = 0; k < N; k++) {
    for (int j = 0; j < N; j++) {
      result[j] += r[k] * o[k][j];
    }
  }
  return result;
}
