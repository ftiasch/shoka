#pragma once

#include <concepts>

template <typename C, typename T>
concept IsComparator = std::predicate<C, T, T>;
