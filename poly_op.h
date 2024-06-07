#pragma once

#include "snippets/min_pow_of_two.h"

template <typename Poly, template <typename> class Impl> struct PolyOp {
  using Mod = typename Poly::Mod;

  template <int index> Poly single(const Poly &f) {
    int n = min_pow_of_two(f.size());
    Poly::reserve(n);
    Poly out(n);
    auto b2 = Poly::template raw_buffer<index>();
    Poly::copy_and_fill0(n, b2, f);
    static_cast<Impl<Poly> *>(this)->_(n, out.data(), b2);
    out.resize(f.size());
    return out;
  }
};

#define SHOKA_HELPER_USING_POLY_OP using Mod = typename Base::Mod;
