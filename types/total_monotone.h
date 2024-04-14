#pragma once

#include <concepts>
#include <utility>

template <typename A>
concept IsTM = requires(A m) {
  {
    m(std::declval<int>(), std::declval<int>())
  } -> std::convertible_to<typename A::E>;
  {
    std::declval<typename A::E>() < std::declval<typename A::E>()
  } -> std::convertible_to<bool>;
};
