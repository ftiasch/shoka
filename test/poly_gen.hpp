#include <tuple>
#include <vector>

template <typename Mod_, typename... Es> struct ContextT {
  using Mod = Mod_;
  using Vector = std::vector<Mod>;

  explicit ContextT()
      : store{typename Es::template StoreT<ContextT>{*this}...} {}

  template <int Index> auto &get() { return std::get<Index>(store); }

private:
  using Store = std::tuple<typename Es::template StoreT<ContextT>...>;

  Store store;
};

// helper

template <typename Ctx, typename P, typename Q> struct BinaryOpStoreT {
  static constexpr bool is_value = (P::template StoreT<Ctx>::is_value) &&
                                   (Q::template StoreT<Ctx>::is_value);

  explicit BinaryOpStoreT(Ctx &ctx_) : ctx{ctx_}, p{ctx_}, q{ctx_} {}

protected:
  Ctx &ctx;
  typename P::template StoreT<Ctx> p;
  typename Q::template StoreT<Ctx> q;
};

// DSL

template <int Index> struct Var {
  template <typename Ctx> struct StoreT {
    static constexpr bool is_value = false;

    explicit StoreT(Ctx &ctx_) : ctx{ctx_} {}

    typename Ctx::Mod operator[](int i) { return ctx.template get<Index>()[i]; }

    Ctx &ctx;
  };
};

struct Const {
  template <typename Ctx> struct StoreT {
    static constexpr bool is_value = true;

    using Vector = typename Ctx::Vector;

    explicit StoreT(Ctx &ctx_) : ctx{ctx_} {}

    void const_set(const Vector &c_) { c = c_; }

    auto operator[](int i) const {
      using Mod = typename Ctx::Mod;

      return i < static_cast<int>(c.size()) ? c[i] : Mod{0};
    }

  private:
    Ctx &ctx;
    Vector c;
  };
};

template <typename P, typename Q> struct Add {
  template <typename Ctx> struct StoreT : public BinaryOpStoreT<Ctx, P, Q> {
    using BinaryOpStoreT<Ctx, P, Q>::BinaryOpStoreT;

    auto operator[](int i) { return this->p[i] + this->q[i]; }
  };
};

template <typename P, typename Q> struct Mul {
  template <typename Ctx> struct StoreT : public BinaryOpStoreT<Ctx, P, Q> {
    using BinaryOpStoreT<Ctx, P, Q>::BinaryOpStoreT;

    auto operator[](int k) {
      typename Ctx::Mod result{0};
      // FIXME: min deg
      for (int i = 0; i < k; ++i) {
        result += this->p[i] * this->q[k - i];
      }
      return result;
    }
  };
};

#include "singleton.h"

#include "mod.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>
#include <catch2/generators/catch_generators.hpp>

TEST_CASE("poly_gen") {
  using Mod = ModT<998'244'353>;

  // f(z) = f(z) * z + 1
  ContextT<Mod, Add<Mul<Var<0>, Var<1>>, Var<2>>, Const, Const> ctx;
  auto &f = ctx.get<0>();
  auto &c1 = ctx.get<1>();
  auto &c2 = ctx.get<2>();
  REQUIRE_FALSE(f.is_value);
  REQUIRE(c1.is_value);
  REQUIRE(c2.is_value);

  c1.const_set(std::vector<Mod>{Mod{0}, Mod{1}});
  c2.const_set(std::vector<Mod>{Mod{1}});
  REQUIRE(f[0] == Mod{1});
  REQUIRE(f[1] == Mod{1});
}
