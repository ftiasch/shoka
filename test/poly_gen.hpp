#include "singleton.h"

#include "mod.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>
#include <catch2/generators/catch_generators.hpp>

template <typename Mod, typename Expr> struct ContextT { 
    
};

using Mod = ModT<998'244'353>;

template <int Index> struct Var;

struct Const;

template <typename P, typename Q> struct BinaryOp {};

template <typename P, typename Q> struct Add : public BinaryOp<P, Q> {};

template <typename P, typename Q> struct Mul : public BinaryOp<P, Q> {};

// f(z) = f(z) * z + 1
using TestExpr = std::tuple<Add<Mul<Var<0>, Var<1>>, Var<2>>, Const, Const>;

TEST_CASE("poly_gen_2") {}
