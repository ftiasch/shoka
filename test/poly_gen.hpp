#include "ntt.h"
#include "singleton.h"

#include <climits>
#include <experimental/type_traits>
#include <stdexcept>
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

// NOTE: Not extend `CacheBaseT` because otherwise it depends on `Ctx`
template <typename Mod> struct DynInvTable {
  explicit DynInvTable() : invs{Mod{0}, Mod{1}} {}

  Mod operator[](int k) {
    while (invs.size() <= k) {
      int i = invs.size();
      invs.push_back(-Mod{Mod::mod() / i} * invs[Mod::mod() % i]);
    }
    return invs[k];
  }

  std::vector<Mod> invs;
};

template <typename Ctx, typename P> struct UnaryOpStoreT {
  static constexpr bool is_value = P::template StoreT<Ctx>::is_value;

  explicit UnaryOpStoreT(Ctx &ctx) : p{ctx} {}

protected:
  typename P::template StoreT<Ctx> p;
};

template <typename Ctx, typename P, typename Q> struct BinaryOpStoreT {
  static constexpr bool is_value = (P::template StoreT<Ctx>::is_value) &&
                                   (Q::template StoreT<Ctx>::is_value);

  explicit BinaryOpStoreT(Ctx &ctx) : p{ctx}, q{ctx} {}

protected:
  typename P::template StoreT<Ctx> p;
  typename Q::template StoreT<Ctx> q;
};

template <typename Ctx, template <typename> typename StoreT> struct CacheBaseT {
  auto operator[](int k) {
    while (delegate_computed() <= k) {
      if (hwm <= k) {
        throw std::logic_error("loop detected");
      }
      hwm = delegate_computed();
      reinterpret_cast<StoreT<Ctx> *>(this)->compute_next();
      hwm = INT_MAX;
    }
    return cache[k];
  }

protected:
  typename Ctx::Vector cache;

private:
  template <typename T>
  using has_computed_t = decltype(std::declval<T>().computed());

  int delegate_computed() const {
    if constexpr (std::experimental::is_detected_v<has_computed_t,
                                                   StoreT<Ctx>>) {
      return reinterpret_cast<const StoreT<Ctx> *>(this)->computed();
    } else {
      return cache.size();
    }
  }

  int hwm = INT_MAX;
};

template <typename Ctx, typename P, typename Q,
          template <typename> typename StoreT>
struct NttMulBaseT : public CacheBaseT<Ctx, StoreT>,
                     public BinaryOpStoreT<Ctx, P, Q> {
  using CacheBaseT<Ctx, StoreT>::cache;
  using BinaryOp = BinaryOpStoreT<Ctx, P, Q>;
  using BinaryOp::p, BinaryOp::q;

  explicit NttMulBaseT(Ctx &ctx)
      : BinaryOp{ctx}, min_deg{p.min_deg + q.min_deg}, max_deg{INT_MAX} {
    cache.resize(1);
  }

  void compute_next() {
    int next = size;
    if (next == cache.size()) {
      auto new_size = cache.size() << 1;
      ntt().reserve(new_size);
      cache.resize(new_size);
      buffer.resize(new_size);
      buffer1.resize(new_size);
    }
    reinterpret_cast<StoreT<Ctx> *>(this)->recur(0, cache.size(), next);
    size++;
  }

  int computed() const { return size; }

  const int min_deg, max_deg;

protected:
  void middle_product(int n, int pbegin, int pend, int qbegin, int qend) {
    copy_and_fill0(n, buffer.data(), p, pbegin, pend);
    copy_and_fill0(n, buffer1.data(), q, qbegin, qend);
    ntt().dif(n, buffer.data());
    ntt().dif(n, buffer1.data());
    auto inv_n = ntt().power_of_two_inv(n);
    for (int i = 0; i < n; ++i) {
      buffer[i] = inv_n * buffer[i] * buffer1[i];
    }
    ntt().dit(n, buffer.data());
  }

  typename Ctx::Vector buffer;

private:
  using Mod = typename Ctx::Mod;
  using Ntt = NttT<Mod, 0>;

  static Ntt &ntt() { return singleton<Ntt>(); }

  template <typename F>
  static void copy_and_fill0(int n, Mod *dst, F &f, int l, int r) {
    for (int i = 0; i < r - l; ++i) {
      dst[i] = f[l + i];
    }
    std::fill(dst + (r - l), dst + n, Ctx::ZERO);
  }

  int size = 0;
  typename Ctx::Vector buffer1;
};

} // namespace poly_gen

template <typename Mod_, int NUM_OF_VAL, typename... Vars> struct PolyCtxT {
  using Mod = Mod_;
  using Vector = std::vector<Mod>;

  using Vals =
      typename poly_gen::ValWrapperT<Mod>::template Factory<NUM_OF_VAL>;

  static constexpr Mod ZERO{0};

  static auto inv(int i) { return singleton<poly_gen::DynInvTable<Mod>>()[i]; }

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

using namespace poly_gen;

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

    typename Ctx::Mod operator[](int i) const { return val()[i]; }

  private:
    auto &val() const { return ctx.template val<Index>(); }

    Ctx &ctx;

  public:
    const int min_deg, max_deg;
  };
};

template <typename P, int S> struct Shift {
  template <typename Ctx> struct StoreT : public UnaryOpStoreT<Ctx, P> {
    using Base = UnaryOpStoreT<Ctx, P>;
    using Base::p;

    explicit StoreT(Ctx &ctx)
        : Base{ctx}, min_deg{p.min_deg + S}, max_deg{std::min(p.max_deg,
                                                              INT_MAX - S) +
                                                     S} {}

    auto operator[](int i) { return i < S ? Ctx::ZERO : p[i - S]; }

    const int min_deg, max_deg;
  };
};

template <typename P> struct Neg {
  template <typename Ctx> struct StoreT : public UnaryOpStoreT<Ctx, P> {
    using Base = UnaryOpStoreT<Ctx, P>;
    using Base::p;

    explicit StoreT(Ctx &ctx)
        : Base{ctx}, min_deg{p.min_deg}, max_deg{p.max_deg} {}

    auto operator[](int i) { return -p[i]; }

    const int min_deg, max_deg;
  };
};

template <typename P> struct Integral {
  template <typename Ctx> struct StoreT : public UnaryOpStoreT<Ctx, P> {
    using Base = UnaryOpStoreT<Ctx, P>;
    using Base::p;

    explicit StoreT(Ctx &ctx_)
        : Base{ctx_}, min_deg{p.min_deg}, max_deg{p.max_deg} {}

    auto operator[](int i) { return i ? p[i - 1] * Ctx::inv(i) : Ctx::ZERO; }

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

template <typename P, typename Q> struct Sub {
  template <typename Ctx> struct StoreT : public BinaryOpStoreT<Ctx, P, Q> {
    using Base = BinaryOpStoreT<Ctx, P, Q>;
    using Base::p, Base::q;

    explicit StoreT(Ctx &ctx)
        : Base{ctx}, min_deg{std::min(p.min_deg, q.min_deg)}, max_deg{std::max(
                                                                  p.max_deg,
                                                                  q.max_deg)} {}

    auto operator[](int i) { return p[i] - q[i]; }

    const int min_deg, max_deg;
  };
};

template <typename P, typename Q> struct LazyMulNoCache {
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
  template <typename Ctx> struct StoreT : public CacheBaseT<Ctx, StoreT> {
    static constexpr bool is_value = P::template StoreT<Ctx>::is_value;

    explicit StoreT(Ctx &ctx)
        : p{ctx}, min_deg{p.min_deg}, max_deg{p.max_deg} {}

    using CacheBaseT<Ctx, StoreT>::cache;

    void compute_next() { cache.push_back(p[cache.size()]); }

  protected:
    typename P::template StoreT<Ctx> p;

  public:
    const int min_deg, max_deg;
  };
};

template <typename P, typename Q> using LazyMul = Cache<LazyMulNoCache<P, Q>>;

template <typename P, typename Q> struct MulSemi {
  template <typename Ctx>
  struct StoreT : public NttMulBaseT<Ctx, P, Q, StoreT> {
    static_assert(Q::template StoreT<Ctx>::is_value, "Q is not a value");

    using Base = NttMulBaseT<Ctx, P, Q, StoreT>;
    using typename Base::NttMulBaseT;

    using Base::cache, Base::p, Base::q, Base::middle_product, Base::buffer;

    void recur(int l, int r, int k) {
      if (l + 1 == r) {
        cache[l] += q.min_deg ? Ctx::ZERO : p[l] * q[0];
      } else {
        auto m = (l + r) >> 1;
        if (k < m) {
          recur(l, m, k);
        } else {
          if (k == m) {
            middle_product(r - l, l, m, 0, r - l);
            for (int i = m; i < r; ++i) {
              cache[i] += buffer[i - l];
            }
          }
          recur(m, r, k);
        }
      }
    }
  };
};

template <typename P, typename Q> struct MulFull {
  template <typename Ctx>
  struct StoreT : public NttMulBaseT<Ctx, P, Q, StoreT> {
    using Base = NttMulBaseT<Ctx, P, Q, StoreT>;
    using typename Base::NttMulBaseT;

    using Base::cache, Base::p, Base::q, Base::middle_product, Base::buffer;

    void recur(int l, int r, int k) {
      if (l + 1 == r) {
        cache[l] += q.min_deg ? Ctx::ZERO : p[l] * q[0];
        cache[l] += !l || p.min_deg ? Ctx::ZERO : p[0] * q[l];
      } else {
        auto m = (l + r) >> 1;
        if (k < m) {
          recur(l, m, k);
        } else {
          if (k == m) {
            middle_product(r - l, l, m, 0, l ? r - l : m - l);
            for (int i = m; i < r; ++i) {
              cache[i] += buffer[i - l];
            }
            if (l) {
              middle_product(r - l, 0, r - l, l, m);
              for (int i = m; i < r; ++i) {
                cache[i] += buffer[i - l];
              }
            }
          }
          recur(m, r, k);
        }
      }
    }
  };
};

} // namespace dsl

#include "mod.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>
#include <catch2/generators/catch_generators.hpp>

template <int Index> using C = dsl::Val<Index>;

namespace dsl {
template <typename P> struct CustomOp {
  template <typename Ctx>
  struct StoreT : public CacheBaseT<Ctx, StoreT>, public UnaryOpStoreT<Ctx, P> {
    using CacheBaseT<Ctx, StoreT>::cache;
    using UnaryOp = UnaryOpStoreT<Ctx, P>;
    using UnaryOp::p;

    explicit StoreT(Ctx &ctx)
        : UnaryOp{ctx}, min_deg{p.min_deg}, max_deg{INT_MAX} {}

    void compute_next() {
      int k = cache.size();
      cache.push_back((k ? (*this)[k - 1] : Ctx::ZERO) + p[k]);
    }

    const int min_deg, max_deg;
  };
};
} // namespace dsl

TEST_CASE("poly_gen") {
  using Mod = ModT<998'244'353>;
  using Vector = std::vector<Mod>;

  using namespace dsl;

  auto take = [&](auto &f, int n) {
    Vector p(n);
    for (int i = 0; i < n; ++i) {
      p[i] = f[i];
    }
    return p;
  };

  SECTION("c") {
    using Ctx = PolyCtxT<Mod, 1, C<0>>;
    Ctx ctx{{Vector{Mod{1}}}};
    auto &f = ctx.var<0>();
    REQUIRE(f.is_value);
    REQUIRE(take(f, 2) == Vector{Mod{1}, Mod{0}});
  }

  SECTION("shift") {
    using Ctx = PolyCtxT<Mod, 1, Shift<C<0>, 1>>;
    Ctx ctx{{Vector{Mod{1}}}};
    auto &f = ctx.var<0>();
    REQUIRE(take(f, 3) == Vector{Mod{0}, Mod{1}, Mod{0}});
  }

  SECTION("neg") {
    using Ctx = PolyCtxT<Mod, 1, Neg<C<0>>>;
    Ctx ctx{{Vector{Mod{1}}}};
    auto &f = ctx.var<0>();
    REQUIRE(take(f, 2) == Vector{-Mod{1}, Mod{0}});
  }

  SECTION("integral") {
    // \int (1 + z) = z + 1/2 z^2
    using Ctx = PolyCtxT<Mod, 1, Integral<C<0>>>;
    Ctx ctx{{Vector{Mod{1}, Mod{1}}}};
    auto &f = ctx.var<0>();
    REQUIRE(take(f, 3) == Vector{Mod{0}, Mod{1}, Mod{2}.inv()});
  }

  SECTION("add") {
    using Ctx = PolyCtxT<Mod, 2, Add<C<0>, C<1>>>;
    Ctx ctx{{Vector{Mod{1}}, Vector{Mod{0}, Mod{2}}}};
    auto &f = ctx.var<0>();
    REQUIRE(take(f, 3) == Vector{Mod{1}, Mod{2}, Mod{0}});
  }

  SECTION("sub") {
    using Ctx = PolyCtxT<Mod, 2, Sub<C<0>, C<1>>>;
    Ctx ctx{{Vector{Mod{1}}, Vector{Mod{0}, Mod{2}}}};
    auto &f = ctx.var<0>();
    REQUIRE(take(f, 3) == Vector{Mod{1}, -Mod{2}, Mod{0}});
  }

  SECTION("custom_op") {
    // partial sum
    using Ctx = PolyCtxT<Mod, 1, CustomOp<C<0>>>;
    Ctx ctx{{Vector{Mod{1}, Mod{2}}}};
    auto &f = ctx.var<0>();
    REQUIRE(take(f, 3) == Vector{Mod{1}, Mod{3}, Mod{3}});
  }

  // f(z) = f(z) * (z + z^2) + 1
  const Vector FIBS_5{Mod{1}, Mod{1}, Mod{2}, Mod{3}, Mod{5}};
  Mod FIB_100000{56136314};

  SECTION("fib_lazy") {
    using Ctx = PolyCtxT<Mod, 2, Add<LazyMul<Var<0>, C<0>>, C<1>>>;
    Ctx ctx{{Vector{Mod{0}, Mod{1}, Mod{1}}, {Mod{1}}}};
    auto &f = ctx.var<0>();
    REQUIRE(take(f, 5) == FIBS_5);
    REQUIRE(f[100000] == FIB_100000);
  }

  SECTION("fib_semi") {
    using Ctx = PolyCtxT<Mod, 2, Add<MulSemi<Var<0>, C<0>>, C<1>>>;
    Ctx ctx{{Vector{Mod{0}, Mod{1}, Mod{1}}, {Mod{1}}}};
    auto &f = ctx.var<0>();
    REQUIRE(take(f, 5) == FIBS_5);
    REQUIRE(f[100000] == FIB_100000);
  }

  SECTION("fib_three_vars") {
    // g(z) = f(z) * z
    // h(z) = f(z) * z^2
    // f(z) = g(z) + h(z) + 1
    using Ctx = PolyCtxT<Mod, 1, Cache<Add<Add<Var<1>, Var<2>>, C<0>>>,
                         Shift<Var<0>, 1>, Shift<Var<0>, 2>>;
    Ctx ctx{{Vector{Mod{1}}}};
    auto &f = ctx.var<0>();
    REQUIRE(take(f, 5) == FIBS_5);
    REQUIRE(f[100000] == FIB_100000);
  }

  SECTION("catalan") {
    // f(z) = f(z) * f(z) * z + 1
    using Ctx = PolyCtxT<Mod, 1, Add<Shift<MulFull<Var<0>, Var<0>>, 1>, C<0>>>;
    Ctx ctx{{Vector{Mod{1}}}};
    auto &f = ctx.var<0>();
    REQUIRE(take(f, 10) == std::vector<Mod>{Mod{1}, Mod{1}, Mod{2}, Mod{5},
                                            Mod{14}, Mod{42}, Mod{132},
                                            Mod{429}, Mod{1430}, Mod{4862}});
    REQUIRE(f[100000] == Mod{944488806});
    // NOTE: verified
    // sage: catalan_number(100000).mod(998244353)
    // 944488806
  }
}
