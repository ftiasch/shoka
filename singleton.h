#pragma once

#include <type_traits>

template <class T, class Enable = void> struct Singleton {};

template <class T>
struct Singleton<T, std::enable_if_t<std::is_default_constructible_v<T>>> {
  static T &instance() {
    static T storage;
    return storage;
  }
};
