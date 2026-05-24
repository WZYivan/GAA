#pragma once

#include <set>

#include <gaa/container/list.hpp>
#include <gaa/core/keywords.hpp>
#include <gaa/wrap/c++/type.hpp>
#include <gaa/wrap/std/variant.hpp>

#include <gaa/core/signature/incomplete.hpp>

namespace gaa {
class Frame {
public:
  using body_t = List<Index, Variants>;
  using head_t = List<Index, String>;
  using info_t = List<>;
  using index_pool_t = std::set<Index>;

private:
  Indices m_indices;
  body_t m_body;
  head_t m_head;
  info_t m_info;
  index_pool_t m_idx_pool;
  Size m_size;

public:
  ~Frame() = default;
  Frame() = default;

  info_t const &info() const;
  info_t &info();

  Indices const &indices() const;
  Size size() const;

  void garbage_collect();

  template <class V> Vector<V> const &col(Index i) const {
    gaa_assert(m_body.is<Vector<V>>, "type of index {} mismatch", i);
    return m_body.at<Vector<V>>(i);
  }

  template <class V> Vector<V> &col(Index i) {
    gaa_assert(m_body.is<Vector<V>>, "type of index {} mismatch", i);
    return m_body.at<Vector<V>>(i);
  }

  template <class V> decltype(auto) append_column(Vector<V> &&v) {
    Index idx;
    if (m_idx_pool.empty()) {
      idx = m_indices.size();
    } else {
      idx = *m_idx_pool.begin();
      m_idx_pool.erase(m_idx_pool.begin());
    }

    return m_body.append(idx, std::forward<Vector<V>>(v));
  }

  template <class Visitor>
  void visit_columns(this auto &&self, Visitor &&visitor) {
    for (auto &&[key, col] : self.m_body) {
      std::visit(visitor, col);
    }
  }

  template <class Visitor>
  void visit_each(this auto &&self, Visitor &&visitor) {
    for (auto &&[key, col] : self.m_body) {
      self.visit_columns(visitor);
    }
  }

  String glimpse() const;
};
} // namespace gaa