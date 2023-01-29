#include <climits>
#include <tuple>
#include <vector>

namespace poly_gen {

/*
 ** NOTE: Value can be referred for multiple times, like `Var`.
 */
template <typename Mod> struct ValWrapperT {
  template <int N> struct Factory {
    using Is = std::array<std::vector<Mod>, N>;
    using T = std::array<ValWrapperT, N>;

    static T create(const Is &cvalues) {
      return create(cvalues, std::make_index_sequence<N>());
    }

  private:
    template <std::size_t... Index>
    static T create(const Is &cvalues, std::index_sequence<Index...>) {
      return {ValWrapperT{cvalues[Index]}...};
    }
  };

  explicit ValWrapperT(const std::vector<Mod> &c_)
      : c{c_}, min_deg{compute_min_deg(c)}, max_deg{static_cast<int>(c.size()) -
                                                    1} {}

  auto operator[](int i) const {
    return i < static_cast<int>(c.size()) ? c[i] : Mod{0};
  }

private:
  static int compute_min_deg(const std::vector<Mod> &c) {
    int deg = 0;
    while (deg < static_cast<int>(c.size()) && c[deg] == Mod{0}) {
      deg++;
    }
    return deg;
  }

  std::vector<Mod> c;

public:
  const int min_deg, max_deg;
};

} // namespace poly_gen

// TODO: Cache
template <typename Mod_, int NUM_OF_VAL, typename... Vars> struct PolyCtxT {
  using Mod = Mod_;
  using Vector = std::vector<Mod>;

  using Vals =
      typename poly_gen::ValWrapperT<Mod>::template Factory<NUM_OF_VAL>;

  explicit PolyCtxT(const typename Vals::Is &vals_)
      : vals{Vals::create(vals_)}, store{
                                       typename Vars::template StoreT<PolyCtxT>{
                                           *this}...} {}

  template <int Index> auto &var() { return std::get<Index>(store); }

  template <int Index> auto &val() const { return vals[Index]; }

private:
  typename Vals::T vals;

  std::tuple<typename Vars::template StoreT<PolyCtxT>...> store;
};

namespace dsl {

template <typename Ctx, typename P, typename Q> struct BinaryOpStoreT {
  static constexpr bool is_value = (P::template StoreT<Ctx>::is_value) &&
                                   (Q::template StoreT<Ctx>::is_value);

  explicit BinaryOpStoreT(Ctx &ctx) : p{ctx}, q{ctx} {}

protected:
  typename P::template StoreT<Ctx> p;
  typename Q::template StoreT<Ctx> q;
};

template <int Index> struct Var {
  template <typename Ctx> struct StoreT {
    static constexpr bool is_value = false;

    explicit StoreT(Ctx &ctx_) : ctx{ctx_} {}

    typename Ctx::Mod operator[](int i) { return ctx.template var<Index>()[i]; }

  private:
    Ctx &ctx;

  public:
    const int min_deg = 0, max_deg = INT_MAX;
  };
};

template <int Index> struct Val {
  template <typename Ctx> struct StoreT {
    static constexpr bool is_value = true;

    using Vector = typename Ctx::Vector;

    explicit StoreT(Ctx &ctx_)
        : ctx{ctx_}, min_deg{val().min_deg}, max_deg{val().max_deg} {}

    using Mod = typename Ctx::Mod;

    Mod operator[](int i) const { return val()[i]; }

  private:
    auto &val() const { return ctx.template val<Index>(); }

    Ctx &ctx;

  public:
    const int min_deg, max_deg;
  };
};

template <typename P, typename Q> struct Add {
  template <typename Ctx> struct StoreT : public BinaryOpStoreT<Ctx, P, Q> {
    using Base = BinaryOpStoreT<Ctx, P, Q>;
    using Base::p, Base::q;

    explicit StoreT(Ctx &ctx)
        : Base{ctx}, min_deg{std::min(p.min_deg, q.min_deg)}, max_deg{std::max(
                                                                  p.max_deg,
                                                                  q.max_deg)} {}

    auto operator[](int i) { return p[i] + q[i]; }

    const int min_deg, max_deg;
  };
};

template <typename P, typename Q> struct LazyMulCompute {
  template <typename Ctx> struct StoreT : public BinaryOpStoreT<Ctx, P, Q> {
    using Base = BinaryOpStoreT<Ctx, P, Q>;
    using Base::p, Base::q;

    explicit StoreT(Ctx &ctx)
        : Base{ctx}, min_deg{p.min_deg + q.min_deg}, max_deg{INT_MAX} {}

    auto operator[](int k) {
      typename Ctx::Mod result{0};
      for (int i = std::max(p.min_deg, k - q.max_deg);
           i <= std::min(p.max_deg, k - q.min_deg); ++i) {
        result += p[i] * q[k - i];
      }
      return result;
    }

    const int min_deg, max_deg;
  };
};

template <typename P> struct Cache {
  template <typename Ctx> struct StoreT {
    static constexpr bool is_value = P::template StoreT<Ctx>::is_value;

    explicit StoreT(Ctx &ctx)
        : p{ctx}, min_deg{p.min_deg}, max_deg{p.max_deg} {}

    auto operator[](int k) {
      while (cache.size() <= k) {
        cache.push_back(p[cache.size()]);
      }
      return cache[k];
    }

  protected:
    typename P::template StoreT<Ctx> p;

    typename Ctx::Vector cache;

  public:
    const int min_deg, max_deg;
  };
};

template <typename P, typename Q> using LazyMul = Cache<LazyMulCompute<P, Q>>;

} // namespace dsl

#include "mod.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>
#include <catch2/generators/catch_generators.hpp>

template <int Index> using C = dsl::Val<Index>;

TEST_CASE("poly_gen") {
  using Mod = ModT<998'244'353>;

  using namespace dsl;

  auto take = [&](auto f, int n) {
    std::vector<Mod> p(n);
    for (int i = 0; i < n; ++i) {
      p[i] = f[i];
    }
    return p;
  };

  SECTION("geo_sum") {
    // f(z) = f(z) * z + 1
    using Ctx = PolyCtxT<Mod, 2, Cache<Add<LazyMul<Var<0>, C<0>>, C<1>>>>;
    Ctx ctx{{std::vector<Mod>{Mod{0}, Mod{1}}, {Mod{1}}}};
    auto &f = ctx.var<0>();
    REQUIRE_FALSE(f.is_value);
    REQUIRE(f[0] == Mod{1});
    REQUIRE(f[1] == Mod{1});
    REQUIRE(f[100000] == Mod{1});
  }

  SECTION("fib") {
    // f(z) = f(z) * (z + z^2) + 1
    using Ctx = PolyCtxT<Mod, 2, Add<LazyMul<Var<0>, C<0>>, C<1>>>;
    Ctx ctx{{std::vector<Mod>{Mod{0}, Mod{1}, Mod{1}}, {Mod{1}}}};
    auto &f = ctx.var<0>();
    REQUIRE(take(f, 5) ==
            std::vector<Mod>{Mod{1}, Mod{1}, Mod{2}, Mod{3}, Mod{5}});
  }

  SECTION("catalan") {
    // f(z) = f(z) * f(z) * z + 1
    using Ctx =
        PolyCtxT<Mod, 2, Add<LazyMul<LazyMul<Var<0>, Var<0>>, C<0>>, C<1>>>;
    Ctx ctx{{std::vector<Mod>{Mod{0}, Mod{1}}, {Mod{1}}}};
    auto &f = ctx.var<0>();
    REQUIRE(take(f, 10) == std::vector<Mod>{Mod{1}, Mod{1}, Mod{2}, Mod{5},
                                            Mod{14}, Mod{42}, Mod{132},
                                            Mod{429}, Mod{1430}, Mod{4862}});
  }
}
