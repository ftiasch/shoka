#pragma once

#include <type_traits>

template <class T> struct Singleton {
  static_assert(std::is_default_constructible_v<T>);

  static T &instance() {
    static T store;
    return store;
  }
};

template <typename T> static inline T &singleton() {
  return Singleton<T>::instance();
}
