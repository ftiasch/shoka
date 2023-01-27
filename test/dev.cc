#include "ntt.h"
#include "singleton.h"

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "debug.h"

template <typename Mod> struct PolyGenT {
private:
  static constexpr int SHORT_ZEALOUS_THRESHOLD = 16;

  using Ntt = NttT<Mod, 0>;

  static Ntt &ntt() { return singleton<Ntt>(); }

  using Vector = std::vector<Mod>;

  struct Op;

  using PtrOp = std::shared_ptr<Op>;

  struct Op {
    explicit Op(int min_deg__, const std::string &name_, bool is_value_ = false)
        : min_deg_{min_deg__}, is_value{is_value_}, name{name_} {}

    virtual ~Op() = default;

    virtual Mod operator[](int) = 0;

    int min_deg() const { return min_deg_; }

    virtual int max_deg() const { return INT_MAX; }

    const std::string name;
    const bool is_value;

  private:
    const int min_deg_;
  };

  struct CachedOp : public Op {
    using Op::Op;

    Mod operator[](int i) override {
      while (computed() <= i) {
        if (hwm <= i) {
          throw std::logic_error("loop detected");
        }
        hwm = computed();
        // std::cerr << ">" << this->name << "[" << hwm << "]" << std::endl;
        compute_next();
        // std::cerr << "<" << this->name << "[" << hwm << "]=" << cache[hwm]
        //           << std::endl;
        hwm = INT_MAX;
      }
      return cache[i];
    }

  protected:
    virtual void compute_next() = 0;

    virtual int computed() const { return cache.size(); }

    Vector cache;

  private:
    int hwm = INT_MAX;
  };

  struct Value : public Op {
    // TODO: add more vector constructors
    explicit Value(const Vector &c_, const std::string &name_)
        : Op{get_min_deg(c_), name_, true}, c{c_} {}

    Mod operator[](int i) override {
      return i < static_cast<int>(c.size()) ? c[i] : Mod{0};
    }

    int max_deg() const override { return static_cast<int>(c.size()) - 1; }

  private:
    static int get_min_deg(const Vector &c) {
      int d = 0;
      while (d < static_cast<int>(c.size()) && c[d] == Mod{0}) {
        d++;
      }
      return d;
    }

    Vector c;
  };

  struct Add : public Op {
    explicit Add(PtrOp p_, PtrOp q_)
        : Op{std::min(p_->min_deg(), q_->min_deg()),
             '(' + p_->name + '+' + q_->name + ')',
             p_->is_value && q_->is_value},
          p{std::move(p_)}, q{std::move(q_)} {}

    Mod operator[](int i) override { return (*p)[i] + (*q)[i]; }

  private:
    PtrOp p, q;
  };

  struct Sub : public Op {
    explicit Sub(PtrOp p_, PtrOp q_)
        : Op{std::min(p_->min_deg(), q_->min_deg()),
             '(' + p_->name + '-' + q_->name + ')',
             p_->is_value && q_->is_value},
          p{std::move(p_)}, q{std::move(q_)} {}

    Mod operator[](int i) override { return (*p)[i] - (*q)[i]; }

  private:
    PtrOp p, q;
  };

  struct Shift : public Op {
    explicit Shift(PtrOp p_, int s_)
        : Op{p_->min_deg() + s_,
             '(' + p_->name + "*z^" + std::to_string(s_) + ')', p_->is_value},
          p{std::move(p_)}, s{s_} {}

    Mod operator[](int i) override { return i < s ? Mod{0} : (*p)[i - s]; }

  private:
    PtrOp p;
    int s;
  };

  struct ZealousConv : public CachedOp {
    explicit ZealousConv(PtrOp p_, PtrOp q_)
        : CachedOp{p_->min_deg() + q_->min_deg(),
                   '(' + p_->name + '*' + q_->name + ')'},
          p{std::move(p_)}, q{std::move(q_)} {}

  private:
    using CachedOp::cache;

    void compute_next() override {
      int k = cache.size();
      Mod result{0};
      for (int i = !!p->min_deg(); i <= k - !!q->min_deg(); ++i) {
        result += (*p)[i] * (*q)[k - i];
      }
      cache.push_back(result);
    }

    PtrOp p, q;
  };

  struct ShortZealousConv : public CachedOp {
    explicit ShortZealousConv(PtrOp p_, PtrOp q_)
        : CachedOp{p_->min_deg() + q_->min_deg(),
                   '(' + p_->name + "*_{short_semi}" + q_->name + ')'},
          p{std::move(p_)}, q{std::move(q_)} {}

  private:
    using CachedOp::cache;

    void compute_next() override {
      int k = cache.size();
      Mod result{0};
      for (int i = std::max<int>(!!p->min_deg(), k - q->max_deg());
           i <= k - !!q->min_deg(); ++i) {
        result += (*p)[i] * (*q)[k - i];
      }
      cache.push_back(result);
    }

    PtrOp p, q;
  };

  struct SemiConv : public CachedOp {
    using CachedOp::cache;

    explicit SemiConv(PtrOp p_, PtrOp q_)
        : CachedOp{p_->min_deg() + q_->min_deg(),
                   '(' + p_->name + "*_{semi}" + q_->name + ')'},
          p{std::move(p_)}, q{std::move(q_)} {
      if (!q->is_value) {
        throw std::logic_error("q is not a value");
      }
      cache.resize(1);
    }

  private:
    void compute_next() override {
      int next = computed_;
      if (next == cache.size()) {
        auto new_size = cache.size() << 1;
        ntt().reserve(new_size);
        cache.resize(new_size);
        buffer_p.resize(new_size);
        buffer_q.resize(new_size);
      }
      recur(0, cache.size(), next);
      computed_++;
    }

    void recur(int l, int r, int k) {
      if (l + 1 == r) {
        cache[l] += q->min_deg() ? Mod{0} : (*p)[l] * (*q)[0];
      } else {
        auto m = (l + r) >> 1;
        if (k < m) {
          recur(l, m, k);
        } else {
          if (k == m) {
            auto n = r - l;
            copy_and_fill0(n, buffer_p.data(), p, l, m);
            copy_and_fill0(n, buffer_q.data(), q, 0, n);
            ntt().dif(n, buffer_p.data());
            ntt().dif(n, buffer_q.data());
            dot_product_and_dit(n, buffer_p.data(), buffer_p.data(),
                                buffer_q.data());
            for (int i = m; i < r; ++i) {
              cache[i] += buffer_p[i - l];
            }
          }
          recur(m, r, k);
        }
      }
    }

    int computed() const override { return computed_; }

    static void copy_and_fill0(int n, Mod *dst, const PtrOp &p, int l, int r) {
      for (int i = 0; i < r - l; ++i) {
        dst[i] = (*p)[l + i];
      }
      std::fill(dst + (r - l), dst + n, Mod{0});
    }

    static void dot_product_and_dit(int n, Mod *out, const Mod *a,
                                    const Mod *b) {
      auto inv_n = ntt().power_of_two_inv(n);
      for (int i = 0; i < n; ++i) {
        out[i] = inv_n * a[i] * b[i];
      }
      ntt().dit(n, out);
    }

    PtrOp p, q;
    int computed_ = 0;
    std::vector<Mod> buffer_p, buffer_q;
  };

  struct Var;

  struct Wrapper final {
    explicit Wrapper(PtrOp op_) : op{std::move(op_)} {}

    Mod operator[](int i) { return (*op)[i]; }

    Wrapper shift(int s) const { return wrap<Shift>(op, s); }

    Wrapper operator+(const Wrapper &o) const { return wrap<Add>(op, o.op); }

    Wrapper operator-(const Wrapper &o) const { return wrap<Sub>(op, o.op); }

    Wrapper operator*(const Wrapper &o) const { return smart_conv(op, o.op); }

  private:
    friend struct Var;

    static Wrapper smart_conv(const PtrOp &p, const PtrOp &q) {
      if (p->is_value) {
        if (q->is_value) {
          throw std::logic_error("Value*Value not supported");
        }
        return smart_semi(q, p);
      } else {
        return q->is_value ? smart_semi(p, q) : wrap<ZealousConv>(p, q);
      }
    }

    static Wrapper smart_semi(const PtrOp &p, const PtrOp &q) {
      return q->max_deg() < SHORT_ZEALOUS_THRESHOLD
                 ? wrap<ShortZealousConv>(p, q)
                 : wrap<SemiConv>(p, q);
    }

    PtrOp op;
  };

  struct Var : public Op {
    explicit Var(const std::string &name_) : Op{0, name_, false} {}

    Mod operator[](int i) override { return (*p.lock())[i]; }

    void delegate(Wrapper w) { p = w.op; }

  private:
    std::weak_ptr<Op> p;
  };

  template <typename T, typename... Args> static Wrapper wrap(Args &&...args) {
    static_assert(std::is_base_of_v<Op, T>);
    return Wrapper{std::make_shared<T>(std::forward<Args>(args)...)};
  }

public:
  static std::pair<Wrapper, std::shared_ptr<Var>>
  var(const std::string &name = "") {
    auto v = std::make_shared<Var>(name);
    return {Wrapper{v}, v};
  }

  static Wrapper value(const Vector &c, const std::string &name = "") {
    return wrap<Value>(c, name);
  }
};

#include "mod.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>
#include <catch2/generators/catch_generators.hpp>

TEST_CASE("poly_gen") {
  using Mod = ModT<998'244'353>;
  using PolyGen = PolyGenT<Mod>;
  using Vector = std::vector<Mod>;

  auto take = [&](auto w, int n) {
    std::vector<Mod> p(n);
    for (int i = 0; i < n; ++i) {
      p[i] = w[i];
    }
    return p;
  };

  SECTION("geo_sum_1") {
    // f(z) = f(z) * z + 1
    auto [f, uf] = PolyGen::var();
    auto rhs = f.shift(1) + PolyGen::value({Mod{1}});
    uf->delegate(rhs);
    REQUIRE(take(f, 2) == Vector{Mod{1}, Mod{1}});
  }

  SECTION("geo_sum_2") {
    // f(z) = f(z) * z + 1
    auto [f, uf] = PolyGen::var();
    auto rhs = f * PolyGen::value({Mod{0}, Mod{1}}) + PolyGen::value({Mod{1}});
    uf->delegate(rhs);
    REQUIRE(take(f, 2) == Vector{Mod{1}, Mod{1}});
  }

  SECTION("geo_sum_short_zealous") {
    // f(z) = f(z) * (z + z^2 + z^3) + 1
    auto [f, uf] = PolyGen::var();
    auto rhs = f * PolyGen::value({Mod{0}, Mod{1}, Mod{1}, Mod{1}}) +
               PolyGen::value({Mod{1}});
    uf->delegate(rhs);
    REQUIRE(take(f, 5) == Vector{Mod{1}, Mod{1}, Mod{2}, Mod{4}, Mod{7}});
  }

  SECTION("fib") {
    // f(z) = f(z) * (z + z^2) + 1
    auto [f, uf] = PolyGen::var();
    auto rhs =
        f * PolyGen::value({Mod{0}, Mod{1}, Mod{1}}) + PolyGen::value({Mod{1}});
    uf->delegate(rhs);
    REQUIRE(take(f, 5) == Vector{Mod{1}, Mod{1}, Mod{2}, Mod{3}, Mod{5}});
    REQUIRE(f[100000] == Mod{56136314});
  }

  SECTION("catalan_1") {
    // f(z) = f(z) * f(z) * z + 1
    auto [f, uf] = PolyGen::var();
    auto rhs =
        (f * f) * PolyGen::value({Mod{0}, Mod{1}}) + PolyGen::value({Mod{1}});
    uf->delegate(rhs);
    REQUIRE(take(f, 5) == Vector{Mod{1}, Mod{1}, Mod{2}, Mod{5}, Mod{14}});
  }

  SECTION("catalan_2") {
    // f(z) = f(z) * f(z) * z + 1
    auto [f, uf] = PolyGen::var();
    auto rhs =
        f * (f * PolyGen::value({Mod{0}, Mod{1}})) + PolyGen::value({Mod{1}});
    uf->delegate(rhs);
    REQUIRE(take(f, 5) == Vector{Mod{1}, Mod{1}, Mod{2}, Mod{5}, Mod{14}});
  }

  SECTION("catalan_3") {
    // f(z) = f(z) * f(z) * z + 1
    auto [f, uf] = PolyGen::var();
    auto rhs =
        f * (PolyGen::value({Mod{0}, Mod{1}}) * f) + PolyGen::value({Mod{1}});
    uf->delegate(rhs);
    REQUIRE(take(f, 5) == Vector{Mod{1}, Mod{1}, Mod{2}, Mod{5}, Mod{14}});
  }
}

/*
#include "poly.h"

#include "singleton.h"

#include <memory>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "debug.h"

template <typename Mod> struct PolyGenT {
private:
  using Poly = PolyT<Mod>;
  using Vector = std::vector<Mod>;
  static_assert(std::is_same_v<typename Poly::Vector, Vector>);

  struct Op;

  using OpPtr = std::shared_ptr<Op>;

  struct Op {
    explicit Op(bool acyclic_, int min_deg()_)
        : acyclic{acyclic_}, min_deg(){min_deg()_} {}

    virtual ~Op() = default;

    virtual Mod at(int i) { return compute(i); }

    virtual void delegate(OpPtr) {}

    const bool acyclic;
    const int min_deg();

  protected:
    virtual Mod compute(int) = 0;
  };

  struct CachedOp : Op {
    using Op::Op;

    Mod at(int i) override {
      if (~next && next <= i) {
        throw std::logic_error("Loop");
      }
      while (cache.size() <= i) {
        next = cache.size();
        cache.push_back(compute(next));
        next = -1;
      }
      return cache[i];
    }

  private:
    using Op::compute;

    int next = -1;
    std::vector<Mod> cache;
  };

  struct Dummy : public CachedOp {
    explicit Dummy() : CachedOp{false, 0} {}

    void delegate(OpPtr op) override { weak = op; }

  private:
    Mod compute(int i) override {
      if (auto d = weak.lock()) {
        return d->at(i);
      } else {
        throw std::runtime_error("The delegator is invalid.");
      }
    }

    std::weak_ptr<Op> weak;
  };

  struct Value : public Op {
    explicit Value(const Vector &c_) : Op{true, get_min_deg()(c_)}, c{c_} {}

  private:
    static int get_min_deg()(const Vector &c) {
      int i = 0;
      while (i < static_cast<int>(c.size()) && c[i] == Mod{0}) {
        i++;
      }
      return i;
    }

    Mod compute(int i) override {
      return i < static_cast<int>(c.size()) ? c[i] : Mod{0};
    }

    Vector c;
  };

  struct Shift : public Op {
    explicit Shift(OpPtr p_, int s_)
        : Op{p_->acyclic, p_->min_deg() + s_}, p{std::move(p_)}, s{s_} {}

  private:
    Mod compute(int i) override { return i < s ? Mod{0} : p->at(i - s); }

    OpPtr p;
    int s;
  };

  struct Add : public Op {
    explicit Add(OpPtr p_, OpPtr q_)
        : Op{p_->acyclic && q_->acyclic, 0}, p{std::move(p_)}, q{std::move(
                                                                   q_)} {}

  private:
    Mod compute(int i) override { return p->at(i) + q->at(i); }

    OpPtr p, q;
  };

  struct Sub : public Op {
    explicit Sub(OpPtr p_, OpPtr q_)
        : Op{p_->acyclic && q_->acyclic, 0}, p{std::move(p_)}, q{std::move(
                                                                   q_)} {}

  private:
    Mod compute(int i) override { return p->at(i) - q->at(i); }

    OpPtr p, q;
  };

  struct ModInv : public CachedOp {
    using Op::at;

    explicit ModInv() : CachedOp{true, 1} {}
2
    Mod compute(int i) override {
      return i < 2 ? Mod{i}
                   : Mod{Mod::mod() - Mod::mod() / i} * at(Mod::mod() % i);
    }
  };

  struct Integral : public Op {
    explicit Integral(OpPtr p_)
        : Op{p_->acyclic, p_->min_deg() + 1}, p{std::move(p_)} {}

  private:
    Mod compute(int i) override {
      return i ? p->at(i - 1) * singleton<ModInv>().at(i) : Mod{0};
    }

    OpPtr p;
  };

  struct Mul : public Op {
    explicit Mul(OpPtr p_, OpPtr q_)
        : Op{true, p_->min_deg() + q_->min_deg()}, p{std::move(p_)},
q{std::move( q_)} {}

  private:
    Mod compute(int i) override {
      if (done.size() <= i) {
        auto n = Poly::min_power_of_two(i + 1);
        Poly::reserve(n << 1);
        Poly pp(n), qq(n);
        for (int i = 0; i < n; ++i) {
          pp[i] = p->at(i);
          qq[i] = q->at(i);
        }
        done = pp * qq;
        done.resize(n);
      }
      return done[i];
    }

    OpPtr p, q;

    Poly done;
  };

  struct OnlineMul : public Op {
    explicit OnlineMul(OpPtr p_, OpPtr q_)
        : Op{false, p_->min_deg() + q_->min_deg()}, p{std::move(p_)},
          q{std::move(q_)}, done{0} {}

  private:
    Mod compute(int i) override {
      if (result.size() <= i) {
        auto size = Poly::min_power_of_two(i + 1);
        Poly::reserve(size);
        result.resize(size);
        buffer[0].resize(size);
        buffer[1].resize(size);
      }
      while (done <= i) {
        recur(0, result.size(), done++);
      }
      return result[i];
    }

  protected:
    virtual void recur(int, int, int) = 0;

    const Mod *middle_product(int size, int pl, int pr, int ql, int qr) {
      auto b0 = buffer[0].data();
      auto b1 = buffer[1].data();
      std::fill(b0, b0 + size, Mod{0});
      for (int i = pl; i < pr; ++i) {
        b0[i - pl] = p->at(i);
      }
      std::fill(b1, b1 + size, Mod{0});
      for (int i = ql; i < qr; ++i) {
        b1[i - ql] = q->at(i);
      }
      Poly::dif(size, b0);
      Poly::dif(size, b1);
      Poly::dot_product_and_dit(size, b0, b0, b1);
      return b0;
    }

    OpPtr p, q;
    int done;
    std::vector<Mod> result;
    std::array<std::vector<Mod>, 2> buffer;
  };

  struct SemiMul : public OnlineMul {
    using OnlineMul::OnlineMul;

  private:
    using OnlineMul::result, OnlineMul::p, OnlineMul::q,
        OnlineMul::middle_product;

    void recur(int l, int r, int k) override {
      if (l + 1 == r) {
        result[l] += q->min_deg() ? Mod{0} : p->at(l) * q->at(0);
      } else {
        auto m = (l + r) >> 1;
        if (k < m) {
          recur(l, m, k);
        } else {
          if (k == m) {
            auto b = middle_product(r - l, l, m, 0, r - l);
            for (int i = m; i < r; ++i) {
              result[i] += b[i - l];
            }
          }
          recur(m, r, k);
        }
      }
    }
  };

  struct FullMul : public OnlineMul {
    using OnlineMul::OnlineMul;

  private:
    using OnlineMul::result, OnlineMul::p, OnlineMul::q,
        OnlineMul::middle_product;

    void recur(int l, int r, int k) override {
      if (l + 1 == r) {
        result[l] += q->min_deg() ? Mod{0} : p->at(l) * q->at(0);
      } else {
        auto m = (l + r) >> 1;
        if (k < m) {
          recur(l, m, k);
        } else {
          if (k == m) {
            auto qlen = l ? r - l : m - l;
            auto b = middle_product(r - l, l, m, 0, qlen);
            for (int i = m; i < r; ++i) {
              result[i] += b[i - l];
            }
            if (l) {
              auto b = middle_product(r - l, 0, r - l, l, m);
              for (int i = m; i < r; ++i) {
                result[i] += b[i - l];
              }
            }
          }
          recur(m, r, k);
        }
      }
    }
  };

  struct Wrapper {
    explicit Wrapper(OpPtr op_) : op{std::move(op_)} {}

    auto operator[](int i) { return op->at(i); }

    auto operator+(const Wrapper &o) const { return wrap<Add>(op, o.op); }

    auto operator-(const Wrapper &o) const { return wrap<Sub>(op, o.op); }

    auto operator*(const Wrapper &o) const { return smart_mul(op, o.op); }

    auto integrate() const { return wrap<Integral>(op); }

    auto shift(int s) const { return wrap<Shift>(op, s); }

    // NOTE: `Wrapper&` prohibits r-values.
    void delegate(Wrapper &o) { op->delegate(o.op); }

  private:
    static Wrapper smart_mul(OpPtr p, OpPtr q) {
      if (p->acyclic) {
        return q->acyclic ? wrap<Mul>(p, q) : wrap<SemiMul>(q, p);
      } else {
        return q->acyclic ? wrap<SemiMul>(p, q) : wrap<FullMul>(p, q);
      }
    }

    OpPtr op;
  };

  template <typename T, typename... Args> static Wrapper wrap(Args &&...args)
{ static_assert(std::is_base_of_v<Op, T>); return
Wrapper{std::make_shared<T>(std::forward<Args>(args)...)};
  }

public:
  static auto value(const std::vector<Mod> &values) {
    return wrap<Value>(values);
  }

  static auto dummy() { return wrap<Dummy>(); }
};

#include "mod.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>
#include <catch2/generators/catch_generators.hpp>

TEST_CASE("poly_gen") {
  using Mod = ModT<998'244'353>;
  using PolyGen = PolyGenT<Mod>;

  auto take = [&](auto w, int n) {
    std::vector<Mod> v(n);
    for (int i = 0; i < n; ++i) {
      v[i] = w[i];
    }
    return v;
  };

  SECTION("const") {
    auto p = PolyGen::value({Mod{1}, Mod{2}});
    REQUIRE(take(p, 5) ==
            std::vector<Mod>{Mod{1}, Mod{2}, Mod{0}, Mod{0}, Mod{0}});
  }

  SECTION("add") {
    auto p = PolyGen::value({Mod{1}, Mod{0}, Mod{3}});
    auto q = PolyGen::value({Mod{0}, Mod{2}});
    auto r = p + q;
    REQUIRE(take(r, 5) ==
            std::vector<Mod>{Mod{1}, Mod{2}, Mod{3}, Mod{0}, Mod{0}});
  }

  SECTION("add_self") {
    auto p = PolyGen::value({Mod{1}});
    auto r = p + p;
    REQUIRE(take(r, 5) ==
            std::vector<Mod>{Mod{2}, Mod{0}, Mod{0}, Mod{0}, Mod{0}});
  }

  SECTION("sub") {
    auto p = PolyGen::value({Mod{2}, Mod{3}});
    auto q = PolyGen::value({Mod{1}});
    auto r = p - q;
    REQUIRE(take(r, 5) ==
            std::vector<Mod>{Mod{1}, Mod{3}, Mod{0}, Mod{0}, Mod{0}});
  }

  SECTION("integral") {
    auto p = PolyGen::value({Mod{2}, Mod{3}});
    auto r = p.integrate();
    REQUIRE(r[0] == Mod{0});
    REQUIRE(r[1] == Mod{2});
    REQUIRE(r[2] * Mod{2} == Mod{3});
  }

  SECTION("shift") {
    auto p = PolyGen::value({Mod{2}, Mod{3}});
    auto r = p.shift(1);
    REQUIRE(take(r, 5) ==
            std::vector<Mod>{Mod{0}, Mod{2}, Mod{3}, Mod{0}, Mod{0}});
  }

  SECTION("recur_geo_sum") {
    // f(z) = f(z) * z^2 + 1
    auto f = PolyGen::dummy();
    auto rhs = f.shift(2) + PolyGen::value({Mod{1}});
    f.delegate(rhs);
    REQUIRE(f[1000000] == Mod{1});
    REQUIRE(f[1000001] == Mod{0});
  }

  SECTION("mul") {
    // (1 + z)^4 = 1 + 4 z + 6 z^2 + 4 z^3 * z^4
    auto f = PolyGen::value({Mod{1}, Mod{1}});
    auto f2 = f * f;
    auto f4 = f2 * f2;
    REQUIRE(take(f4, 10) == std::vector<Mod>{Mod{1}, Mod{4}, Mod{6}, Mod{4},
                                             Mod{1}, Mod{0}, Mod{0}, Mod{0},
                                             Mod{0}, Mod{0}});
  }

  SECTION("recur_fib") {
    // f(z) = f(z) * (z + z^2) + 1
    auto f = PolyGen::dummy();
    auto rhs = (f * PolyGen::value({Mod{1}, Mod{1}})).shift(1) +
               PolyGen::value({Mod{1}});
    f.delegate(rhs);
    REQUIRE(take(f, 6) ==
            std::vector<Mod>{Mod{1}, Mod{1}, Mod{2}, Mod{3}, Mod{5}, Mod{8}});
    REQUIRE(f[100000] == Mod{56136314});
  }

  SECTION("recur_fib_fg") {
    // f(z) = g(z) * z + 1
    // g(z) = f(z) * (1 + z)
    auto f = PolyGen::dummy();
    auto g = PolyGen::dummy();
    auto grhs = f * PolyGen::value({Mod{1}, Mod{1}});
    g.delegate(grhs);
    auto frhs = g.shift(1) + PolyGen::value({Mod{1}});
    f.delegate(frhs);
    REQUIRE(take(f, 6) ==
            std::vector<Mod>{Mod{1}, Mod{1}, Mod{2}, Mod{3}, Mod{5}, Mod{8}});
  }

  SECTION("recur_catalan") {
    // f(z) = f * f * z + 1
    auto f = PolyGen::dummy();
    auto rhs = (f * f).shift(1) + PolyGen::value({Mod{1}});
    f.delegate(rhs);
    REQUIRE(take(f, 6) ==
            std::vector<Mod>{Mod{1}, Mod{1}, Mod{2}, Mod{5}, Mod{14},
Mod{42}});
  }
}
*/
