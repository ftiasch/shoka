#pragma once

#include "singleton.h"

#include <experimental/type_traits>

#include <array>
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <vector>

template <typename T, int Phantom = 0>
struct MatrixT : public std::vector<std::vector<T>> {
  using Row = std::vector<T>;

  static int dim() { return singleton<Store>().dim; }

  static void set_dim(int dim_) { singleton<Store>().set_dim(dim_); }

  MatrixT() : std::vector<std::vector<T>>(dim(), std::vector<T>(dim())) {}

  static MatrixT mul_id() {
    MatrixT e;
    for (int i = 0; i < dim(); ++i) {
      if constexpr (has_mul_id) {
        e[i][i] = T::mul_id();
      } else {
        e[i][i] = 1;
      }
    }
    return e;
  }

  MatrixT &operator+=(const MatrixT &o) {
    for (int i = 0; i < dim(); ++i) {
      for (int j = 0; j < dim(); ++j) {
        (*this)[i][j] += o[i][j];
      }
    }
    return *this;
  }

  MatrixT operator+(const MatrixT &o) const { return MatrixT(*this) += o; }

  MatrixT &operator-=(const MatrixT &o) {
    for (int i = 0; i < dim(); ++i) {
      for (int j = 0; j < dim(); ++j) {
        (*this)[i][j] -= o[i][j];
      }
    }
    return *this;
  }

  MatrixT operator-(const MatrixT &o) const { return MatrixT(*this) -= o; }

  MatrixT operator*(const MatrixT &o) const {
    auto &buffer = singleton<Store>().buffer;
    MatrixT result;
    for (int j = 0; j < dim(); ++j) {
      for (int i = 0; i < dim(); ++i) {
        buffer[i] = o[i][j];
      }
      for (int i = 0; i < dim(); ++i) {
        for (int k = 0; k < dim(); ++k) {
          result[i][j] += (*this)[i][k] * buffer[k];
        }
      }
    }
    return result;
  }

  MatrixT &operator*=(const MatrixT &o) { return *this = *this * o; }

private:
  template <typename U> using has_mul_id_t = decltype(U::mul_id());
  static constexpr bool has_mul_id =
      std::experimental::is_detected_v<has_mul_id_t, T>;

  struct Store {
    void set_dim(int dim_) {
      dim = dim_;
      buffer.resize(dim_);
    }

    int dim;
    std::vector<T> buffer;
  };
};

template <typename T, int Phantom>
auto operator*(const typename MatrixT<T, Phantom>::Row &r,
               const MatrixT<T, Phantom> &o) {
  typename MatrixT<T, Phantom>::Row result;
  for (int k = 0; k < o.dim(); k++) {
    for (int j = 0; j < o.dim(); j++) {
      result[j] += r[k] * o[k][j];
    }
  }
  return result;
}
