#pragma once

#include <concepts>

template <typename M>
concept IsTreeMonoid = requires(M a, M b) {
  { M::rake(a, b) } -> std::convertible_to<M>;
  { M::compress(a, b) } -> std::convertible_to<M>;
};
