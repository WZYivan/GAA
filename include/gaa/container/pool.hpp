#pragma once

#include <set>

namespace gaa {
template <class T>
  requires requires(T a, T b) {
    { a == b } -> std::convertible_to<bool>;
    { a = a + 1 };
    { a = 0 };
  }
class Pool {
private:
  std::set<T> m_deprecated;
  T m_size = 0;

public:
  Pool() = default;
  ~Pool() = default;

  void increase() { m_size = m_size + 1; }

  bool has_deprecated() const { return !m_deprecated.empty(); }
  bool is_deprecated(T const &t) const { return m_deprecated.contains(t); }

  T extract_one_deprecated() {
    T r = *m_deprecated.begin();
    m_deprecated.erase(m_deprecated.begin());
    return r;
  }

  decltype(auto) get() {
    if (this->has_deprecated()) {
      return std::make_pair(this->extract_one_deprecated(), true);
    } else {
      T r = m_size;
      this->increase();
      return std::make_pair(r, false);
    }
  }

  void deprecate(T v) { m_deprecated.insert(v); }
  void erase(T v) { m_deprecated.erase(v); }
  void clear() { m_deprecated.clear(); }
};
} // namespace gaa