#pragma once

#include <map>
#include <ranges>

#include <gaa/container/bidirectional_map.hpp>
#include <gaa/container/pool.hpp>
#include <gaa/core/keywords.hpp>
#include <gaa/core/kw.hpp>
#include <gaa/wrap/std/any.hpp>
#include <gaa/wrap/std/concepts.hpp>

namespace gaa {
/// brief:
/// A heterogeneous associative container (dictionary) mapping string keys to
/// Variant values. It supports insertion, deletion (by marking), name-to-index
/// bidirectional lookup, garbage collection (compaction), and various views on
/// valid indices. Internally, it uses a pool for index reuse, a sorted list of
/// indices, a std::map for data storage, and a bidirectional map for name-index
/// mapping. The container is designed for small to medium-sized collections
/// with dynamic key-value pairs of different types (supported by Variant).
/// Access by key or index is provided, with operator[] for easy
/// insertion/defaulting. Erased entries are only marked until garbage_collect()
/// is called to physically remove them. The class also provides range adaptors
/// for filtering (valid, scalar, vector indices).
class List {
public:
  using Index = std::size_t;
  using Indices = std::vector<Index>;
  using Index_pool = Pool<Index>;
  using Data = std::map<Index, Any>;
  using Name = Bidirectional_map<Index, std::string>;
  using key_t = std::string;

private:
  Index_pool m_idx_pool;
  Indices m_indices;
  Data m_data;
  Name m_name;

public:
  bool has_key(key_t const &k) const;

  decltype(auto) valid_indices() const {
    return m_indices | std::views::filter([&](auto const &i) {
             return !m_idx_pool.is_deprecated(i);
           });
  }
  decltype(auto) all_indices() const {
    return m_indices | std::views::as_const;
  }
  decltype(auto) vector_indices() const {
    return m_indices | std::views::filter([&](auto const &i) {
             return this->at(i).info().is_vector;
           });
  }
  decltype(auto) scalar_indices() const {
    return m_indices | std::views::filter([&](auto const &i) {
             return !this->at(i).info().is_vector;
           });
  }

  void garbage_collect();

  template <class V> decltype(auto) insert_or_assign(key_t const &k, V &&v) {
    auto [i, dep] = m_idx_pool.get();
    if (!dep) {
      m_indices.emplace_back(i);
    }
    m_name.insert_or_assign(i, k);
    return m_data.insert_or_assign(i, std::forward<V>(v));
  }
  template <class V>
  decltype(auto) insert_or_assign(key_t const &k,
                                  std::initializer_list<V> &&il) {
    auto [i, dep] = m_idx_pool.get();
    if (!dep) {
      m_indices.emplace_back(i);
    }
    m_name.insert_or_assign(i, k);
    return m_data.insert_or_assign(i, std::vector<V>(il));
  }

  Any &operator[](key_t const &k);
  Any const &operator[](key_t const &k) const;
  Any &operator[](Index k);
  Any const &operator[](Index k) const;
  Any &at(key_t const &k);
  Any const &at(key_t const &k) const;
  Any &at(Index i);
  Any const &at(Index i) const;

  Index index_of(key_t const &k) const;
  key_t const &name_of(Index idx) const;

  std::type_info const &type_info(Index i) const;
  std::type_info const &type_info(key_t const &k) const;

  void erase(Index idx);
  void erase(key_t const &k);

  std::string glimpse(bool with_typename = false) const;
};
} // namespace gaa