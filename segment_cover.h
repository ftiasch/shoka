#include <concepts>
#include <functional>
#include <limits>
#include <map>

template <typename T, std::integral K = int> class SegmentCover {
  void cut(K k) {
    auto it = std::prev(segs.upper_bound(k));
    if (it->first < k) {
      segs[k] = it->second;
    }
  }

public:
  explicit SegmentCover(T v) {
    segs[std::numeric_limits<K>::min()] = segs[std::numeric_limits<K>::max()] =
        v;
  }

  // set [l, r) to v
  void cover(K l, K r, T v, const std::function<void(K, K, T)> &cb) {
    cut(l);
    cut(r);
    auto it = segs.find(l);
    while (it->first < r) {
      auto iit = std::next(it);
      cb(it->first, iit->first, it->second);
      segs.erase(it);
      it = iit;
    }
    segs[l] = v;
  }

  std::map<K, T> segs;
};
