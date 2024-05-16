#pragma once

#include <concepts>
#include <utility>

template <typename A, typename E>
concept IsTM2 = requires(A m, int i, int j) {
  { m(i, j) } -> std::same_as<E>;
} && requires(E u, E v) {
  { u < v } -> std::convertible_to<bool>;
};

template <typename A>
concept IsTM = requires(A m) {
  typename A::E;
  requires IsTM2<A, typename A::E>;
};
