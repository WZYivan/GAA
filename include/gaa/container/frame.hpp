#pragma once

#include <gaa/container/list.hpp>
#include <gaa/container/pool.hpp>
#include <gaa/core/keywords.hpp>
#include <gaa/wrap/std/any.hpp>

namespace gaa {
class Frame {
public:
  using Index = std::size_t;
  using Indices = std::vector<Index>;
  using Index_pool = Pool<Index>;
  using Data = std::vector<Any>;
  using Name = Bidirectional_map<Index, std::string>;

private:
  Indices m_indices;
  Index_pool m_idx_pool;
  Data m_data;
  Name m_name;
  std::size_t m_size;

public:
  ~Frame() = default;
  Frame(std::size_t size);

  decltype(auto) indices() const {
    return m_indices | std::views::filter([&](Index i) -> bool {
             return !m_idx_pool.is_deprecated(i);
           });
  }
  decltype(auto) names() const {
    return this->indices() | std::views::transform([&](auto i) -> std::string {
             return this->name_of(i);
           });
  }
  std::size_t rows() const;
  std::size_t cols() const;

  void garbage_collect();
  std::string glimpse() const;

  std::string const &name_of(Index i) const;
  Index index_of(std::string const &k) const;

  template <class T> std::vector<T> const &col(Index i) const {
    gaa_assert(this->cols() > i, "index {} out of range {}", i, this->cols());
    Any const &vec = m_data.at(i);
    gaa_assert(vec.is<std::vector<T>>(),
               "type mismatch, any_cast will fail (given {})",
               typeid(std::vector<T>).name());
    return vec.as<std::vector<T>>();
  }
  template <class T> std::vector<T> &col(Index i) {
    gaa_assert(this->cols() > i, "index {} out of range {}", i, this->cols());
    Any &vec = m_data.at(i);
    gaa_assert(vec.is<std::vector<T>>(),
               "type mismatch, any_cast will fail (given {})",
               typeid(std::vector<T>).name());
    return vec.as<std::vector<T>>();
  }
  template <class T> std::vector<T> const &col(std::string const &k) const {
    return this->col<T>(this->index_of(k));
  }
  template <class T> std::vector<T> &col(std::string const &k) {
    return this->col<T>(this->index_of(k));
  }
  Any const &col(Index i) const;
  Any &col(Index i);
  Any const &col(std::string const &k) const;
  Any &col(std::string const &k);

  template <class T>
  void new_col(std::string const &k, std::vector<T> const &vec) {
    auto [i, dep] = m_idx_pool.get();

    m_name.insert_or_assign(i, k);
    if (!dep) {
      m_indices.emplace_back(i);
      m_data.emplace_back(vec);
    } else {
      m_data.at(i) = vec;
    }

    std::vector<T> &new_col = m_data.at(i).as<std::vector<T>>();
    if (new_col.size() != this->rows()) {
      new_col.resize(this->rows());
    }
  }
  template <class T> void new_col(std::string const &k, std::vector<T> &&vec) {
    auto [i, dep] = m_idx_pool.get();

    m_name.insert_or_assign(i, k);
    if (!dep) {
      m_indices.emplace_back(i);
      m_data.emplace_back(std::forward<std::vector<T>>(vec));
    } else {
      m_data.at(i) = std::forward<std::vector<T>>(vec);
    }

    std::vector<T> &new_col = m_data.at(i).as<std::vector<T>>();
    if (new_col.size() != this->rows()) {
      new_col.resize(this->rows());
    }
  }
  template <class T> void new_col(std::string const &k) {
    this->new_col<T>(k, std::vector<T>{});
  }
  void new_col(std::string const &k, Literal_Type literal);

  void erase_col(Index i);
  void erase_col(std::string const &k);
  void rename_col(Index i, std::string const &new_name);
  void rename_col(std::string const &k, std::string const &new_name);

  template <class FrameP>
    requires std::same_as<std::decay_t<FrameP>, Frame>
  class Row {
  private:
    std::reference_wrapper<FrameP> m_frame;
    std::size_t m_row;

  public:
    ~Row() = default;
    Row(std::size_t row, FrameP &ref) : m_frame(ref), m_row(row) {}

    std::size_t size() const { return m_frame.get().cols(); }

    template <class T> T &at(Index i) {
      return m_frame.get().template col<T>(i).at(m_row);
    }
    template <class T> T const &at(Index i) const {
      return m_frame.get().template col<T>(i).at(m_row);
    }

    template <class T> T &at(std::string const &k) {
      return m_frame.get().template col<T>(k).at(m_row);
    }
    template <class T> T const &at(std::string const &k) const {
      return m_frame.get().template col<T>(k).at(m_row);
    }
  };

  Row<Frame> row(Index i);
  Row<Frame const> row(Index i) const;
};
} // namespace gaa