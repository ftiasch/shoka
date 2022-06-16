#pragma once

#include "poly.h"

template <template <typename> class Impl, typename NTT> struct PolyOp {
  using Factory = PolyFactory<NTT>;
  using Mod = typename Factory::Mod;
  using Poly = typename Factory::Poly;

  template <int index> Poly single(const Poly &f) {
    int n = Factory::min_power_of_two(f.size());
    factory->reserve(n);
    Poly out = factory->make(n);
    Mod *const b2 = factory->template raw_buffer<index>();
    Factory::copy_and_fill0(n, b2, f);
    static_cast<Impl<NTT> *>(this)->_(n, out.data(), b2);
    out.resize(f.size());
    return out;
  }

  std::shared_ptr<Factory> factory;
};
