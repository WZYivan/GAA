#pragma once

#include <any>
#include <format>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <boost/mpl/bool.hpp>

#include <gaa/core/config.hpp>
#include <gaa/core/keywords.hpp>
#include <gaa/core/kw.hpp>
#include <gaa/core/pp.hpp>

namespace gaa {

enum Table_Storage_Flag : int {
  Tab_Unsupported = 0,
  Tab_String,
  Tab_Double,
  Tab_Integer,
  Tab_Bool,
  Tab_COUNT
};

template <class V> struct Is_table_storageble : public boost::mpl::false_ {};
template <class V>
inline static constexpr bool is_table_storageble_v =
    Is_table_storageble<V>::value;
template <class V> struct _table_storage_flag {
  static constexpr Table_Storage_Flag value = Tab_Unsupported;
};
template <class V>
  requires Is_table_storageble<V>::value
static inline constexpr Table_Storage_Flag storage_flag_of =
    _table_storage_flag<V>::value;
template <Table_Storage_Flag flag>
  requires(flag != Tab_Unsupported && flag != Tab_COUNT)
struct Table_storage_type {
  using type = void;
};
template <Table_Storage_Flag flag>
using Table_storage_type_of = Table_storage_type<flag>::type;

#define GAA_register_storageble(TYPE, FLAG, ALIAS)                             \
  template <>                                                                  \
  struct Is_table_storageble<GAA_PP_STRIP_PARAM TYPE>                          \
      : public boost::mpl::true_ {};                                           \
                                                                               \
  template <> struct _table_storage_flag<GAA_PP_STRIP_PARAM TYPE> {            \
    static constexpr Table_Storage_Flag value = FLAG;                          \
    static_assert(value > Tab_Unsupported && value < Tab_COUNT);               \
  };                                                                           \
  template <> struct Table_storage_type<FLAG> {                                \
    using type = GAA_PP_STRIP_PARAM TYPE;                                      \
  };                                                                           \
  using ALIAS = Table_storage_type_of<FLAG>

GAA_register_storageble((std::string), Tab_String, Tab_string);
GAA_register_storageble((double), Tab_Double, Tab_double);
GAA_register_storageble((int), Tab_Integer, Tab_int);
GAA_register_storageble((bool), Tab_Bool, Tab_bool);

#undef GAA_register_storageble

class Table {
public:
  template <class Value> using linear_storage_type = std::vector<Value>;
  template <class K, class V> using map_storage_type = std::map<K, V>;
  using bool_seq_type = std::vector<bool>;

  template <class Value> using column_type = std::vector<Value>;
  using table_storage_type = std::vector<std::any>;
  using column_name_type = std::string;
  using column_name_storage_type = std::vector<column_name_type>;
  using index_type = unsigned long;

  using meta_info_key_type = std::string;
  using meta_info_storage_type = std::any;
  using mate_info_type = std::map<meta_info_key_type, meta_info_storage_type>;

  using table_storage_info_type = std::vector<Table_Storage_Flag>;
  using mate_storage_info_type =
      std::map<meta_info_key_type, Table_Storage_Flag>;

private:
  mate_info_type m_meta_storage;
  table_storage_type m_table_storage;
  column_name_storage_type m_table_col_names;

  mate_storage_info_type m_meta_info;
  table_storage_info_type m_table_info;

public:
  Table() = default;
  ~Table() = default;

  template <class V>
    requires Is_table_storageble<V>::value
  struct Storage_traits {
    static constexpr Table_Storage_Flag storage_flag = storage_flag_of<V>;
    using storage_column_type = column_type<V>;
    using column_ptr_type = std::add_pointer_t<storage_column_type>;
    using column_const_ptr_type = std::add_const_t<column_ptr_type>;
  };

  table_storage_type const &columns() const { return m_table_storage; }
  table_storage_type &columns() { return m_table_storage; }

  template <class V>
    requires is_table_storageble_v<V>
  void push_back(column_type<V> &&col, column_name_type const &name) {
    this->self_check();
    m_table_storage.emplace_back(std::move(col));
    m_table_info.emplace_back(Storage_traits<V>::storage_flag);
    m_table_col_names.emplace_back(name);
  }

  template <class V>
    requires is_table_storageble_v<V>
  void push_back(column_type<V> &&col) {
    this->push_back(std::forward<column_type<V>>(col),
                    std::format("COL_{:d}", m_table_storage.size()));
  }

  template <class V>
    requires is_table_storageble_v<V>
  void push_back(column_type<V> const &col, column_name_type const &name) {
    this->self_check();
    m_table_storage.emplace_back(col);
    m_table_info.emplace_back(Storage_traits<V>::storage_flag);
    m_table_col_names.emplace_back(name);
  }

  template <class V>
    requires is_table_storageble_v<V>
  void push_back(column_type<V> const &col) {
    this->push_back(col, std::format("COL_{:d}", m_table_storage.size()));
  }

  template <class V> column_type<V> const &at(index_type const &idx) const {
    this->self_check();

    gaa_assert(m_table_storage.size() > idx, "Index out of range");
    std::any const &wrapped = m_table_storage.at(idx);
    std::any const *any_ptr = std::addressof(wrapped);
    Table_Storage_Flag flag = m_table_info.at(idx);

    using traits = Storage_traits<V>;
    gaa_assert(traits::storage_flag == flag, "Invalid reinterpret");

    typename traits::column_const_ptr_type cptr =
        std::any_cast<typename traits::storage_column_type>(any_ptr);
    return *cptr;
  }
  template <class V>
  column_type<V> const &at(column_name_type const &name) const {
    return this->at<V>(this->column_of(name));
  }

  template <class V> column_type<V> &at(index_type const &idx) {
    this->self_check();

    gaa_assert(m_table_storage.size() > idx, "Index out of range");
    std::any &wrapped = m_table_storage.at(idx);
    std::any *any_ptr = std::addressof(wrapped);
    Table_Storage_Flag flag = m_table_info.at(idx);

    using traits = Storage_traits<V>;
    gaa_assert(traits::storage_flag == flag, "Invalid reinterpret");

    typename traits::column_ptr_type ptr =
        std::any_cast<typename traits::storage_column_type>(any_ptr);
    return *ptr;
  }
  template <class V> column_type<V> &at(column_name_type const &name) {
    return this->at<V>(this->column_of(name));
  }

  template <class V>
    requires is_table_storageble_v<V>
  bool meta_ioa(meta_info_key_type const &key, V const &v) {
    auto [it0, ins0] = m_meta_storage.insert_or_assign(key, v);
    auto [it1, ins1] =
        m_meta_info.insert_or_assign(key, Storage_traits<V>::storage_flag);
    return ins0 && ins1;
  }

  template <class V>
    requires is_table_storageble_v<V>
  decltype(auto) meta_ioa(meta_info_key_type const &key, V &&v) {
    auto [it0, ins0] = m_meta_storage.insert_or_assign(key, std::move(v));
    auto [it1, ins1] =
        m_meta_info.insert_or_assign(key, Storage_traits<V>::storage_flag);
    return ins0 && ins1;
  }

  template <class V>
    requires is_table_storageble_v<V>
  V const &meta_at(meta_info_key_type const &key) const {
    gaa_assert(m_meta_info.contains(key));
    gaa_assert(m_meta_storage.contains(key));
    std::any const &wrapped = m_meta_storage.at(key);
    std::any const *any_ptr = std::addressof(wrapped);

    using traits = Storage_traits<V>;
    gaa_assert(traits::storage_flag == m_meta_info.at(key),
               "Invalid reinterpret");
    V const *cptr = std::any_cast<V>(any_ptr);
    return *cptr;
  }

  template <class V>
    requires is_table_storageble_v<V>
  V &meta_at(meta_info_key_type const &key) {
    gaa_assert(m_meta_info.contains(key));
    gaa_assert(m_meta_storage.contains(key));
    std::any &wrapped = m_meta_storage.at(key);
    std::any *any_ptr = std::addressof(wrapped);

    using traits = Storage_traits<V>;
    gaa_assert(traits::storage_flag == m_meta_info.at(key),
               "Invalid reinterpret");
    V *ptr = std::any_cast<V>(any_ptr);
    return *ptr;
  }

  void clear();
  bool empty() const;
  std::size_t size() const;
  void self_check() const;
  std::ptrdiff_t column_of(column_name_type const &name) const;
  bool has_column(column_name_type const &name) const;

  class Row_view {
  private:
    Table const &m_tab;
    index_type m_row;

  public:
    ~Row_view() = default;
    Row_view(Table const &tab, index_type row) : m_tab(tab), m_row(row) {}

    template <class V> V const &at(index_type const &idx) const {
      auto const &col = m_tab.at<V>(idx);
      gaa_assert(col.size() > m_row, "Index out of range");
      return col.at(m_row);
    }

    template <class V> V const &at(column_name_type const &name) const {
      return this->at<V>(m_tab.column_of(name));
    }
  };

  Row_view row(index_type const &idx) const;
  Row_view row(column_name_type const &name) const;

  class Row_ref {
  private:
    Table &m_tab;
    index_type m_row;

  public:
    ~Row_ref() = default;
    Row_ref(Table &tab, index_type row) : m_tab(tab), m_row(row) {}

    template <class V> V &at(index_type const &idx) {
      auto &col = m_tab.at<V>(idx);
      gaa_assert(col.size() > m_row, "Index out of range");
      return col.at(m_row);
    }

    template <class V> V &at(column_name_type const &name) {
      return this->at<V>(m_tab.column_of(name));
    }

    template <class V> V const &at(index_type const &idx) const {
      auto const &col = m_tab.at<V>(idx);
      gaa_assert(col.size() > m_row, "Index out of range");
      return col.at(m_row);
    }

    template <class V> V const &at(column_name_type const &name) const {
      return this->at<V>(m_tab.column_of(name));
    }
  };

  Row_ref row(index_type const &idx);
  Row_ref row(column_name_type const &name);
};

using Table_row_view = Table::Row_view;
using Table_row_ref = Table::Row_ref;

template <class V>
  requires is_table_storageble_v<V>
using Table_column = Table::column_type<V>;
using Table_column_name = Table::column_name_type;

extern Table read_csv(std::string const &fname,
                      std::vector<Table_Storage_Flag> const &flags,
                      kwargs args = {});
extern Table read_csv(std::istream &is,
                      std::vector<Table_Storage_Flag> const &flags,
                      kwargs args = {});
extern Table read_csv_auto(std::string const &fname, kwargs args = {});
extern Table read_csv_auto(std::istream &is, kwargs args = {});
} // namespace gaa