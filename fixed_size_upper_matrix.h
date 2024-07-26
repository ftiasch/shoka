#pragma once

#include <array>
#include <cstring>
#include <initializer_list>

template <typename T, int N>
struct FixedSizeUpperMatrixT : std::array<std::array<T, N>, N> {
  using Row = std::array<T, N>;

  using std::array<Row, N>::array;

  explicit FixedSizeUpperMatrixT(std::initializer_list<Row> rows)
      : std::array<Row, N>{
            reinterpret_cast<const std::array<Row, N> &>(*rows.begin())} {}

  static FixedSizeUpperMatrixT mul_id() {
    FixedSizeUpperMatrixT e;
    for (int i = 0; i < N; ++i) {
      e[i][i] = T::mul_id();
    }
    return e;
  }

  FixedSizeUpperMatrixT &operator+=(const FixedSizeUpperMatrixT &o) {
    for (int i = 0; i < N; ++i) {
      for (int j = i; j < N; ++j) {
        (*this)[i][j] += o[i][j];
      }
    }
    return *this;
  }

  FixedSizeUpperMatrixT operator+(const FixedSizeUpperMatrixT &o) const {
    return FixedSizeUpperMatrixT(*this) += o;
  }

  FixedSizeUpperMatrixT &operator-=(const FixedSizeUpperMatrixT &o) {
    for (int i = 0; i < N; ++i) {
      for (int j = i; j < N; ++j) {
        (*this)[i][j] -= o[i][j];
      }
    }
    return *this;
  }

  FixedSizeUpperMatrixT operator-(const FixedSizeUpperMatrixT &o) const {
    return FixedSizeUpperMatrixT(*this) -= o;
  }

  FixedSizeUpperMatrixT operator*(const FixedSizeUpperMatrixT &o) const {
    static T buffer[N];
    FixedSizeUpperMatrixT result;
    for (int j = 0; j < N; ++j) {
      for (int i = 0; i < N; ++i) {
        buffer[i] = o[i][j];
      }
      for (int i = 0; i <= j; ++i) {
        for (int k = i; k <= j; ++k) {
          result[i][j] += (*this)[i][k] * buffer[k];
        }
      }
    }
    return result;
  }

  FixedSizeUpperMatrixT &operator*=(const FixedSizeUpperMatrixT &o) {
    return *this = *this * o;
  }
};

template <typename T, int N>
auto operator*(const typename FixedSizeUpperMatrixT<T, N>::Row &r,
               const FixedSizeUpperMatrixT<T, N> &o) {
  typename FixedSizeUpperMatrixT<T, N>::Row result;
  for (int k = 0; k < N; k++) {
    for (int j = k; j < N; j++) {
      result[j] += r[k] * o[k][j];
    }
  }
  return result;
}
