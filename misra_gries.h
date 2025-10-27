#pragma once

#include <algorithm>
#include <array>
#include <utility>

template <typename T, size_t K>
struct MisraGriesT : public std::array<std::pair<T, int>, K - 1> {
  using Pair = std::pair<T, int>;

  // Removing $k$ distinct elements, the elements that appears more than 1/k
  // still appears more than 1/k times.
  explicit MisraGriesT() = default;

  explicit MisraGriesT(const T &value) { (*this)[0] = {value, 1}; }

  MisraGriesT operator+(const MisraGriesT &o) const {
    static std::array<Pair, 2 * (K - 1)> buffer;
    int size = 0;
    for (int i = 0; i < K - 1; i++) {
      if ((*this)[i].second) {
        buffer[size++] = (*this)[i];
      }
    }
    for (int i = 0; i < K - 1; i++) {
      if (o[i].second) {
        buffer[size++] = o[i];
      }
    }
    std::sort(buffer.begin(), buffer.begin() + size);
    int new_size = 0;
    for (int i = 0; i < size; i++) {
      if (!new_size || buffer[new_size - 1].first != buffer[i].first) {
        buffer[new_size++] = buffer[i];
      } else {
        buffer[new_size - 1].second += buffer[i].second;
      }
    }
    size = new_size;
    std::sort(buffer.begin(), buffer.begin() + size,
              [](const Pair &p, const Pair &q) { return p.second > q.second; });
    int sub = 0;
    for (int i = size; i--;) {
      if (i + K < size) {
        sub -= buffer[i + K].second;
      }
      buffer[i].second -= sub;
      if (K - 1 <= i) {
        sub += buffer[i].second;
      }
    }
    MisraGriesT result;
    std::copy(buffer.begin(), buffer.begin() + std::min<size_t>(K - 1, size),
              result.begin());
    return result;
  }
};
