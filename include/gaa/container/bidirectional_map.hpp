#pragma once

#include <map>

namespace gaa {
template <class K, class V> class Bidirectional_map {
public:
  using KV = std::map<K, V>;
  using VK = std::map<V, K>;

  KV m_kv;
  VK m_vk;

public:
  Bidirectional_map() = default;
  ~Bidirectional_map() = default;

  template <class Kp, class Vp>
    requires std::convertible_to<std::decay_t<Kp>, K> ||
             std::convertible_to<std::decay_t<Kp>, V>
  void insert_or_assign(Kp &&k, Vp &&v) {
    m_kv.insert_or_assign(k, v);
    m_vk.insert_or_assign(v, k);
  }

  std::size_t size() const { return kv().size(); }

  V const &at(K const &k) const { return this->kv().at(k); }
  K const &at(V const &v) const { return this->vk().at(v); }

  KV const &kv() const { return m_kv; }
  VK const &vk() const { return m_vk; }

  void erase(K const &k) {
    V const &v = this->kv().at(k);
    m_vk.erase(v);
    m_kv.erase(k);
  }
};
} // namespace gaa