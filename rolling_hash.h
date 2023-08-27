#pragma once

#include "singleton.h"
#include "zip_with.h"

#include <iostream>
#include <random>
#include <tuple>
#include <type_traits>
#include <utility>

template <typename... Mods> struct RollingHashT {
  using Hash = std::tuple<Mods...>;

  template <typename Gen> static void initialize(Gen &gen, int reserved = 0) {
    singleton<SeedStore>().set_seed(gen, reserved);
  }

  static Hash seed() { return singleton<SeedStore>().powers[1]; }

  explicit RollingHashT() = default;

  explicit RollingHashT(const Hash &hash_, int length_ = 1)
      : hash{hash_}, length{length_} {}

  template <typename T>
  explicit RollingHashT(T v)
      : hash{make_with([v]<typename Mod>(Mod) { return Mod{v}; })}, length{1} {
    static_assert(std::is_integral_v<T>);
  }

  bool operator==(const RollingHashT &o) const { return hash == o.hash; }

  bool operator<(const RollingHashT &o) const { return get() < o.get(); }

  RollingHashT operator+(const RollingHashT &o) const {
    return RollingHashT{
        zip_with([]<typename Mod>(Mod h, Mod p, Mod oh) { return h * p + oh; },
                 hash, singleton<SeedStore>().get_power(o.length), o.hash),
        length + o.length};
  }

  RollingHashT operator-(const RollingHashT &o) const {
    return RollingHashT{
        zip_with([]<typename Mod>(Mod h, Mod oh, Mod p) { return h - oh * p; },
                 hash, o.hash,
                 singleton<SeedStore>().get_power(length - o.length)),
        length - o.length};
  }

  Hash hash;
  int length;

private:
  template <typename Fun> static inline Hash make_with(Fun &&fun) {
    return zip_with(std::forward<Fun>(fun), Hash{});
  }

  auto get() const {
    return zip_with([]<typename Mod>(Mod h) { return h.get(); }, hash);
  }

  struct SeedStore {
    template <typename Gen> void set_seed(Gen &gen, int r) {
      auto one = make_with([]<typename Mod>(Mod) { return Mod::mul_id(); });
      auto seed = make_with([&gen]<typename Mod>(Mod) {
        return Mod{std::uniform_int_distribution<typename Mod::M>{
            0, Mod::mod() - 1}(gen)};
      });
      powers = {one, seed};
      get_power(r);
    }

    Hash get_power(int n) {
      auto old_size = powers.size();
      if (old_size <= n) {
        powers.resize(n + 1);
        for (int i = old_size; i <= n; ++i) {
          powers[i] =
              zip_with([]<typename Mod>(Mod x, Mod y) -> Mod { return x * y; },
                       powers[i - 1], powers[1]);
        }
      }
      return powers[n];
    }

    std::vector<Hash> powers;
  };
};

namespace std {

template <typename... Mods>
ostream &operator<<(ostream &out, const RollingHashT<Mods...> &h) {
  return out << h.hash << ':' << h.length;
}

} // namespace std
