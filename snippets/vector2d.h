#pragma once

#include <vector>

template <typename T> struct Vector2D : public std::vector<std::vector<T>> {
  using size_type = typename std::vector<T>::size_type;

  explicit Vector2D(size_type n = 0, size_type m = 0, T t = T{})
      : std::vector<std::vector<T>>(n, std::vector<T>(m, t)) {}
};
