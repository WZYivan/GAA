#pragma once

#include <any>
#include <format>
#include <map>
#include <string>

#include <boost/property_map/property_map.hpp>

#include <gaa/core/keywords.hpp>

namespace gaa {
class Associative_property_maps_collection {
public:
  using key_type = std::string;
  using storage_type = std::map<std::string, std::any>;

  template <class K, class V> using map_type = std::map<K, V>;
  template <class K, class V>
  using map_interface_type = boost::associative_property_map<map_type<K, V>>;

private:
  storage_type m_db;

public:
  ~Associative_property_maps_collection() = default;
  Associative_property_maps_collection() = default;

  bool contains(key_type const &key) { return m_db.contains(key); }

  template <class K, class V> decltype(auto) add(key_type const &key) {
    gaa_assert(!this->contains(key), "key {} already exists", key);
    gaa_assert(m_db.insert_or_assign(key, map_type<K, V>{}).second,
               "insert map failed");
    return this->template at<K, V>(key);
  }

  decltype(auto) erase(key_type const &key) {
    gaa_assert(this->contains(key));
    return m_db.erase(key);
  }

  template <class K, class V> decltype(auto) at(key_type const &key) {
    gaa_assert(this->contains(key), "key {} not exists", key);
    map_type<K, V> *storage_ptr = std::any_cast<map_type<K, V>>(&m_db.at(key));
    return boost::make_assoc_property_map(*storage_ptr);
  }

  storage_type &storage() { return m_db; }
  storage_type const &storage() const { return m_db; }
  std::any &storage_at(key_type const &key) { return m_db.at(key); }
  std::any const &storage_at(key_type const &key) const { return m_db.at(key); }
};
} // namespace gaa