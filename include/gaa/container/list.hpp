#pragma once

#include <map>
#include <ranges>

#include <gaa/container/bidirectional_map.hpp>
#include <gaa/container/pool.hpp>
#include <gaa/core/keywords.hpp>
#include <gaa/wrap/std/concepts.hpp>
#include <gaa/wrap/std/variant.hpp>

namespace gaa {
class List {
public:
  using Index = std::size_t;
  using Indices = std::vector<Index>;
  using Index_pool = Pool<Index>;
  using Data = std::map<Index, Variant>;
  using Name = Bidirectional_map<Index, std::string>;
  using key_t = std::string;

private:
  Index_pool m_idx_pool;
  Indices m_indices;
  Data m_data;
  Name m_name;

public:
  bool has_key(key_t const &k) const { return m_name.vk().contains(k); }

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
             return is_vector(this->at(i));
           });
  }
  decltype(auto) scalar_indices() const {
    return m_indices | std::views::filter([&](auto const &i) {
             return !is_vector(this->at(i));
           });
  }

  void garbage_collect() {
    Indices alive = this->valid_indices() | std::ranges::to<Indices>();
    for (Index new_i = 0uz; new_i != alive.size(); ++new_i) {
      Index old_i = alive[new_i];
      if (new_i == old_i) {
        continue;
      } else {
        m_indices[new_i] = new_i;

        m_data.insert_or_assign(new_i, m_data.at(old_i));
        m_data.erase(old_i);

        m_name.insert_or_assign(new_i, m_name.at(old_i));
        m_name.erase(old_i);
      }
    }
    m_indices.resize(alive.size());
    m_idx_pool.clear();
  }

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

  Variant &operator[](key_t const &k) {
    if (this->has_key(k)) {
      return this->at(k);
    } else {
      this->insert_or_assign(k, "<default construct by operator[]>");
      return this->at(k);
    }
  }
  Variant const &operator[](key_t const &k) const {
    if (this->has_key(k)) {
      return this->at(k);
    } else {
      gaa_fail("key \'{}\' not exists", k);
    }
  }
  Variant &operator[](Index k) { return this->operator[](this->name_of(k)); }
  Variant const &operator[](Index k) const {
    return this->operator[](this->name_of(k));
  }

  Variant &at(key_t const &k) {
    Index i = m_name.at(k);
    return this->at(i);
  }
  Variant const &at(key_t const &k) const {
    Index i = m_name.at(k);
    return this->at(i);
  }
  Variant &at(Index i) { return m_data.at(i); }
  Variant const &at(Index i) const { return m_data.at(i); }

  Index index_of(key_t const &k) const { return m_name.at(k); }
  key_t const &name_of(Index idx) const { return m_name.at(idx); }

  Variable variable_of(Index i) const { return this->at(i).variable(); }
  Variable variable_of(key_t const &k) const { return this->at(k).variable(); }

  void erase(Index idx) { m_idx_pool.deprecate(idx); }
  void erase(key_t const &k) { erase(this->index_of(k)); }

  std::string glimpse() const {
    std::string cnt;
    auto out = std::back_inserter(cnt);
    auto visit = make_visitor::format_to(out);
    for (auto const &[idx, v] : m_data) {
      if (m_idx_pool.is_deprecated(idx)) {
        std::format_to(out, "(x) ");
      }
      std::format_to(out, "[{}] {} <{}>: ", idx, this->name_of(idx),
                     enum2str(v.variable()));
      visit(v);
      *out = '\n';
    }
    return cnt;
  }
};
} // namespace gaa