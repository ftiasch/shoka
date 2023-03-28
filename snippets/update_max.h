#pragma once

#include <algorithm>

template <typename T> void update_max(T &x, T a) { x = std::max(x, a); }
