#pragma once

#include "singleton.h"

#include <random>

template <typename Mod> struct RollingHashT {
  template <typename Gen> static void initialize(Gen &gen, int reserved = 0) {
    singleton<Store>().set_seed(
        Mod{std::uniform_int_distribution<typename Mod::M>{0, Mod::mod() -
                                                                  1}(gen)},
        reserved);
  }

  RollingHashT() : h{0}, len{0} {}

  RollingHashT(Mod h_) : h{h_}, len{1} {}

  int get_len() const { return len; }

  bool operator==(const RollingHashT &o) const { return h == o.h; }

  RollingHashT operator+(const RollingHashT &o) const {
    return RollingHashT{h * singleton<Store>().get_power(o.len) + o.h,
                        len + o.len};
  }

  RollingHashT operator-(const RollingHashT &o) const {
    return RollingHashT{h - o.h * singleton<Store>().get_power(len - o.len),
                        len - o.len};
  }

private:
  explicit RollingHashT(Mod h_, int len_) : h{h_}, len{len_} {}

  struct Store {
    void set_seed(Mod seed, int r) {
      powers = {Mod{1}, seed};
      get_power(r);
    }

    Mod get_power(int n) {
      auto old_size = powers.size();
      if (old_size <= n) {
        powers.resize(n + 1);
        for (int i = old_size; i <= n; ++i) {
          powers[i] = powers[i - 1] * powers[1];
        }
      }
      return powers[n];
    }

    std::vector<Mod> powers;
  };

  Mod h;
  int len;
};
