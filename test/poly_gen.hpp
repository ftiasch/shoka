#include <tuple>
#include <vector>

// TODO: min_deg
// TODO: LazyMul
// TODO: Cache
template <typename Mod_, typename... Es> struct ContextT {
  using Mod = Mod_;
  using Vector = std::vector<Mod>;

  using Values = std::vector<std::vector<Mod>>;

  explicit ContextT(Values &&cvalues_)
      : values{std::move(cvalues_)}, store{
                                         typename Es::template StoreT<ContextT>{
                                             *this}...} {}

  template <int Index> auto &get_value() const { return values[Index]; }

  template <int Index> auto &get() { return std::get<Index>(store); }

private:
  using Store = std::tuple<typename Es::template StoreT<ContextT>...>;

  Values values;
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

template <int Index> struct C {
  template <typename Ctx> struct StoreT {
    static constexpr bool is_value = true;

    using Vector = typename Ctx::Vector;

    explicit StoreT(Ctx &ctx_) : ctx{ctx_} {}

    using Mod = typename Ctx::Mod;

    Mod operator[](int i) const {
      auto &c = ctx.template get_value<Index>();
      return i < c.size() ? c[i] : Mod{0};
    }

  private:
    Ctx &ctx;
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
  using GF = ContextT<Mod, Add<Mul<Var<0>, C<0>>, C<1>>>;

  GF ctx{{std::vector<Mod>{Mod{0}, Mod{1}}, std::vector<Mod>{Mod{1}}}};

  auto &f = ctx.get<0>();
  REQUIRE_FALSE(f.is_value);
  REQUIRE(f[0] == Mod{1});
  REQUIRE(f[1] == Mod{1});
}
