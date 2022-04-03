#include <array>
#include <cstring>
#include <vector>

template <typename T, size_t N> struct FixedSizeMatrixT {
  FixedSizeMatrixT() { memset(data, 0, sizeof(data)); }

  T *operator[](int i) { return data[i]; }

  const T *operator[](int i) const { return data[i]; }

  FixedSizeMatrixT &operator+=(const FixedSizeMatrixT &o) {
    for (int i = 0; i < N; ++i) {
      for (int j = 0; j < N; ++j) {
        data[i][j] += o[i][j];
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
        data[i][j] -= o[i][j];
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
          result[i][j] += data[i][k] * buffer[k];
        }
      }
    }
    return result;
  }

private:
  T data[N][N];
};
