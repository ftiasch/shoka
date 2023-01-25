#include <memory>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include <iostream>

template <typename Mod> struct PolyGenT {
  using Vector = std::vector<Mod>;

  struct Op {
    virtual ~Op() = default;

    virtual Mod compute(int) = 0;

    virtual Mod at(int i) { return compute(i); }
  };

  struct CachedOp : public Op {
    using Op::compute;

    Mod at(int i) override {
      while (cache.size() <= i) {
        cache.push_back(compute(cache.size()));
      }
      return cache[i];
    }

  private:
    std::vector<Mod> cache;
  };

  using OpPtr = std::shared_ptr<Op>;

  struct Dummy : public CachedOp {
    void delegate(OpPtr op) { weak = op; }

    Mod compute(int i) override {
      if (auto d = weak.lock()) {
        return d->at(i);
      } else {
        throw std::runtime_error("The delegator is invalid.");
      }
    }

    std::weak_ptr<Op> weak;
  };

  struct Const : public Op {
    explicit Const(const Vector &c_) : c{c_} {}

    Mod compute(int i) override {
      return i < static_cast<int>(c.size()) ? c[i] : Mod{0};
    }

    Vector c;
  };

  struct Shift : public Op {
    explicit Shift(OpPtr p_, int s_) : p{std::move(p_)}, s{s_} {}

    Mod compute(int i) override { return i < s ? Mod{0} : p->at(i - s); }

    OpPtr p;
    int s;
  };

  struct Add : public Op {
    explicit Add(OpPtr p_, OpPtr q_) : p{std::move(p_)}, q{std::move(q_)} {}

    Mod compute(int i) override { return p->at(i) + q->at(i); }

    OpPtr p, q;
  };

  struct Sub : public Op {
    explicit Sub(OpPtr p_, OpPtr q_) : p{std::move(p_)}, q{std::move(q_)} {}

    Mod compute(int i) override { return p->at(i) - q->at(i); }

    OpPtr p, q;
  };

  template <typename T> struct Wrapper {
    static_assert(std::is_base_of_v<Op, T>);

    template <typename... Args>
    explicit Wrapper(Args &&...args)
        : op{std::make_shared<T>(std::forward<Args>(args)...)} {}

    auto operator[](int i) { return op->at(i); }

    template <typename U> auto operator+(const Wrapper<U> &o) {
      return Wrapper<Add>{op, o.op};
    }

    template <typename U> auto operator-(const Wrapper<U> &o) {
      return Wrapper<Sub>{op, o.op};
    }

    auto shift(int s) { return Wrapper<Shift>(op, s); }

    // NOTE: Not using const reference to prohibit r-values.
    template <typename U> void delegate(Wrapper<U> &o) { op->delegate(o.op); }

    std::shared_ptr<T> op;
  };

  static auto const_(const Vector &c) { return Wrapper<Const>{c}; }

  static auto dummy() { return Wrapper<Dummy>{}; }
};

#include "mod.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>
#include <catch2/generators/catch_generators.hpp>

TEST_CASE("poly_gen") {
  using Mod = ModT<998'244'353>;
  using PolyGen = PolyGenT<Mod>;

  SECTION("const") {
    auto p = PolyGen::const_({Mod{1}});
    REQUIRE(p[0] == Mod{1});
    REQUIRE(p[1] == Mod{0});
  }

  SECTION("add") {
    auto p = PolyGen::const_({Mod{1}, Mod{0}, Mod{3}});
    auto q = PolyGen::const_({Mod{0}, Mod{2}});
    auto r = p + q;
    REQUIRE(r[0] == Mod{1});
    REQUIRE(r[1] == Mod{2});
    REQUIRE(r[2] == Mod{3});
    REQUIRE(r[3] == Mod{0});
  }

  SECTION("add_self") {
    auto p = PolyGen::const_({Mod{1}});
    auto r = p + p;
    REQUIRE(r[0] == Mod{2});
    REQUIRE(r[1] == Mod{0});
  }

  SECTION("sub") {
    auto p = PolyGen::const_({Mod{2}, Mod{3}});
    auto q = PolyGen::const_({Mod{1}});
    auto r = p - q;
    REQUIRE(r[0] == Mod{1});
    REQUIRE(r[1] == Mod{3});
    REQUIRE(r[2] == Mod{0});
  }

  SECTION("shift") {
    auto p = PolyGen::const_({Mod{2}, Mod{3}});
    auto r = p.shift(1);
    REQUIRE(r[0] == Mod{0});
    REQUIRE(r[1] == Mod{2});
    REQUIRE(r[2] == Mod{3});
  }

  SECTION("recur_1") {
    // f(z) = f(z) * z^2 + 1
    auto one = PolyGen::const_({Mod{1}});
    auto f = PolyGen::dummy();
    auto rhs = f.shift(2) + one;
    f.delegate(rhs);
    REQUIRE(f[1000000] == Mod{1});
    REQUIRE(f[1000001] == Mod{0});
  }
}
