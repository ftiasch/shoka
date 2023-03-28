#pragma once

#include <algorithm>

template <typename T> void update_min(T &x, T a) { x = std::min(x, a); }
