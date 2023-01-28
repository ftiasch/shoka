#include <tuple>
#include <vector>

// TODO: min_deg
// TODO: LazyMul
// TODO: Cache
template <typename Mod_, int ConstN, typename... Es> struct ContextT {
  using Mod = Mod_;
  using Vector = std::vector<Mod>;

  using CValue = Vector;
  using CValues = std::array<CValue, ConstN>;

  explicit ContextT(CValues &&cvalues_)
      : cvalues{std::move(cvalues_)},
        store{typename Es::template StoreT<ContextT>{*this}...} {}

  template <int Index> auto &var() { return std::get<Index>(store); }

  template <int Index> auto const_at(int i) const {
    const auto &c = cvalues[Index];
    return i < static_cast<int>(c.size()) ? c[i] : Mod{0};
  }

private:
  using Store = std::tuple<typename Es::template StoreT<ContextT>...>;

  CValues cvalues;
  Store store;
};

// helper

template <typename Ctx, typename P, typename Q> struct BinaryOpStoreT {
  static constexpr bool is_value = (P::template StoreT<Ctx>::is_value) &&
                                   (Q::template StoreT<Ctx>::is_value);

  explicit BinaryOpStoreT(Ctx &ctx) : p{ctx}, q{ctx} {}

protected:
  typename P::template StoreT<Ctx> p;
  typename Q::template StoreT<Ctx> q;
};

// DSL

template <int Index> struct Var {
  template <typename Ctx> struct StoreT {
    static constexpr bool is_value = false;

    explicit StoreT(Ctx &ctx_) : ctx{ctx_} {}

    typename Ctx::Mod operator[](int i) { return ctx.template var<Index>()[i]; }

    Ctx &ctx;
  };
};

template <int Index> struct Const {
  template <typename Ctx> struct StoreT {
    static constexpr bool is_value = true;

    using Vector = typename Ctx::Vector;

    explicit StoreT(Ctx &ctx_) : ctx{ctx_} {}

    using Mod = typename Ctx::Mod;

    Mod operator[](int i) const { return ctx.template const_at<Index>(i); }

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

template <int Index> using C = Const<Index>;

TEST_CASE("poly_gen") {
  using Mod = ModT<998'244'353>;

  // f(z) = f(z) * z + 1
  using GF = ContextT<Mod, 2, Add<Mul<Var<0>, Const<0>>, Const<1>>>;

  GF ctx{{std::vector<Mod>{Mod{0}, Mod{1}}, {Mod{1}}}};

  auto &f = ctx.var<0>();
  REQUIRE_FALSE(f.is_value);
  REQUIRE(f[0] == Mod{1});
  REQUIRE(f[1] == Mod{1});
}
