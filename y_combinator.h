#pragma once

#include <functional>

namespace y_combinator_details {

template <class Fun> struct y_combinator_result {
  template <class T>
  explicit y_combinator_result(T &&fun_) : fun(std::forward<T>(fun_)) {}

  template <class... Args> decltype(auto) operator()(Args &&...args) {
    return fun(std::ref(*this), std::forward<Args>(args)...);
  }

private:
  Fun fun;
};

} // namespace y_combinator_details

template <class Fun> static inline decltype(auto) y_combinator(Fun &&fun) {
  return y_combinator_details::y_combinator_result<std::decay_t<Fun>>(
      std::forward<Fun>(fun));
}
