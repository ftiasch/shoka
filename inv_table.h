#include <vector>

template <typename ModT> struct InvTable : std::vector<ModT> {
  explicit InvTable(int n) : std::vector<ModT>(n) {
    (*this)[1] = ModT{1};
    for (int i = 2; i < n; ++i) {
      (*this)[i] = ModT(ModT::MOD - ModT::MOD / i) * (*this)[ModT::MOD % i];
    }
  }
};
