#pragma once

#include "ntt.h"
#include "singleton.h"

#include <climits>
#include <experimental/type_traits>
#include <stdexcept>
#include <tuple>
#include <vector>

namespace poly_gen {

template <typename Mod> static auto &ntt() { return singleton<NttT<Mod, 0>>(); }

template <typename Mod, typename Impl> struct PrefixDifT {
  explicit PrefixDifT() : cache(1) {}

  void resize(int n) {
    cache.resize(n << 1);
    ntt<Mod>().reserve(n);
    for (int i = 0; i < n; ++i) {
      cache[n + i] = (*static_cast<Impl *>(this))[i];
    }
    ntt<Mod>().dif(n, cache.data() + n);
  }

  const Mod *prefix_dif(int l) const { return cache.data() + l; }

private:
  std::vector<Mod> cache;
};

/*
 ** NOTE: Value can be referred for multiple times, like `Var`.
 */
template <typename Mod>
struct ValStoreT : public PrefixDifT<Mod, ValStoreT<Mod>> {
  template <int N> struct Factory {
    using Is = std::array<std::vector<Mod>, N>;
    using T = std::array<ValStoreT, N>;

    static T create(const Is &cvalues) {
      return create(cvalues, std::make_index_sequence<N>());
    }

  private:
    template <std::size_t... Index>
    static T create(const Is &cvalues, std::index_sequence<Index...>) {
      return {ValStoreT{cvalues[Index]}...};
    }
  };

  explicit ValStoreT(const std::vector<Mod> &c_)
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
  static constexpr bool is_value = false;

  explicit UnaryOpStoreT(Ctx &ctx) : p{ctx} {}

protected:
  typename P::template StoreT<Ctx> p;
};

template <typename Ctx, typename P, typename Q> struct BinaryOpStoreT {
  static constexpr bool is_value = false;

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
  using Mod = typename Ctx::Mod;

  explicit NttMulBaseT(Ctx &ctx)
      : BinaryOp{ctx}, min_deg{p.min_deg + q.min_deg}, max_deg{INT_MAX} {
    cache.resize(1);
  }

  void compute_next() {
    int next = size;
    if (next == cache.size()) {
      auto new_size = cache.size() << 1;
      ntt<Mod>().reserve(new_size);
      log_cache_size++;
      cache.resize(new_size);
      buffer.resize(new_size);
      buffer1.resize(new_size);
      if constexpr (std::experimental::is_detected_v<has_resize, StoreT<Ctx>>) {
        return reinterpret_cast<StoreT<Ctx> *>(this)->resize(new_size);
      }
    }
    if (next) {
      int z = __builtin_ctz(next);
      reinterpret_cast<StoreT<Ctx> *>(this)->cross(next - (1 << z), next,
                                                   next + (1 << z));
    }
    reinterpret_cast<StoreT<Ctx> *>(this)->self(next);
    // reinterpret_cast<StoreT<Ctx> *>(this)->recur(0, cache.size(), next);
    size++;
  }

  int computed() const { return size; }

  const int min_deg, max_deg;

protected:
  template <typename F>
  static void copy_and_fill0(int n, Mod *dst, F &f, int l, int r) {
    for (int i = 0; i < r - l; ++i) {
      dst[i] = f[l + i];
    }
    std::fill(dst + (r - l), dst + n, Ctx::ZERO);
  }

  void middle_product(int n, int pbegin, int pend, int qbegin, int qend) {
    copy_and_fill0(n, buffer.data(), p, pbegin, pend);
    copy_and_fill0(n, buffer1.data(), q, qbegin, qend);
    ntt<Mod>().dif(n, buffer.data());
    ntt<Mod>().dif(n, buffer1.data());
    auto inv_n = ntt<Mod>().power_of_two_inv(n);
    for (int i = 0; i < n; ++i) {
      buffer[i] = inv_n * buffer[i] * buffer1[i];
    }
    ntt<Mod>().dit(n, buffer.data());
  }

  typename Ctx::Vector buffer, buffer1;

private:
  template <typename T>
  using has_resize = decltype(std::declval<T>().resize(std::declval<int>()));

  int size = 0, log_cache_size = 0;
};

} // namespace poly_gen

template <typename Mod_, int NUM_OF_VAL, typename... Vars> struct PolyCtxT {
  using Mod = Mod_;
  using Vector = std::vector<Mod>;
  using Vals = typename poly_gen::ValStoreT<Mod>::template Factory<NUM_OF_VAL>;

  static constexpr Mod ZERO{0};

  static auto inv(int i) { return singleton<poly_gen::DynInvTable<Mod>>()[i]; }

  explicit PolyCtxT(const typename Vals::Is &vals_)
      : vals{Vals::create(vals_)}, store{
                                       typename Vars::template StoreT<PolyCtxT>{
                                           *this}...} {}

  template <int Index> auto &var() { return std::get<Index>(store); }

  template <int Index> auto &val_store() { return vals[Index]; }

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
        : ctx{ctx_}, min_deg{store().min_deg}, max_deg{store().max_deg} {}

    auto &store() { return ctx.template val_store<Index>(); }

    auto operator[](int i) { return store()[i]; }

  private:
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
    static constexpr bool is_value = false;

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
    static_assert(Q::template StoreT<Ctx>::is_value, "Q is not Val");

    using Base = NttMulBaseT<Ctx, P, Q, StoreT>;
    using typename Base::NttMulBaseT;

    using Base::cache, Base::p, Base::q, Base::copy_and_fill0, Base::buffer,
        Base::buffer1;

    void resize(int new_size) { q.store().resize(new_size); }

    void self(int i) { cache[i] += q.min_deg ? Ctx::ZERO : p[i] * q[0]; }

    void cross(int l, int m, int r) {
      using Mod = typename Ctx::Mod;
      auto n = r - l;
      copy_and_fill0(n, buffer.data(), p, l, m);
      ntt<Mod>().dif(n, buffer.data());
      auto q_prefix_dif = q.store().prefix_dif(n);
      auto inv_n = ntt<Mod>().power_of_two_inv(n);
      for (int i = 0; i < n; ++i) {
        buffer[i] = inv_n * buffer[i] * q_prefix_dif[i];
      }
      ntt<Mod>().dit(n, buffer.data());
      for (int i = m; i < r; ++i) {
        cache[i] += buffer[i - l];
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

    void self(int i) {
      cache[i] += q.min_deg ? Ctx::ZERO : p[i] * q[0];
      cache[i] += !i || p.min_deg ? Ctx::ZERO : p[0] * q[i];
    }

    void cross(int l, int m, int r) {
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
  };
};

} // namespace dsl
