#include <ranges>

#include <boost/regex.hpp>

#include <gaa/core/table.hpp>

namespace gaa {

std::string table_storage_flag_string(Table_Storage_Flag flag) {
  switch (flag) {
  case Tab_String:
    return "<str>";
  case Tab_Double:
    return "<dbl>";
  case Tab_Integer:
    return "<int>";
  case Tab_Bool:
    return "<bool>";
  case Tab_Char:
    return "<chr>";
  case Tab_UTC_Id:
    return "<utc_id>";
  case Tab_Sat_Sys:
    return "<sat_sys>";
  case Tab_Vector_String:
    return "<vec_str>";
  case Tab_Vector_Double:
    return "<vec_dbl>";
  case Tab_Vector_Integer:
    return "<vec_int>";
  default:
    gaa_assert(false, "unreachable default case");
  }
}

std::string Table::glimpse() const {
  std::string cnt;
  std::string tmp;

  static constexpr auto storage2str
      [[maybe_unused]] = []<class T>(T const &val) {
        if constexpr (std::same_as<UTC_Identifier, T> ||
                      std::same_as<Satellite_System, T>) {
          return enum2str(val);
        } else {
          return val;
        }
      };

  for (auto const &[label, info, storage] :
       std::views::zip(m_table_col_names, m_table_info, m_table_storage)) {
    auto dest = std::back_inserter(tmp);

    std::format_to(dest, "\"{}\" {} ", label, table_storage_flag_string(info));
    visit_table_storage(info, storage, [&dest, &tmp](auto const &vals) {
      for (auto const &v : vals) {
        std::format_to(dest, "{}, ", storage2str(v));
        if (tmp.size() > 96) {
          break;
        }
      }
    });
    tmp.pop_back();
    tmp.pop_back();

    if (tmp.size() > 96) {
      tmp.resize(96);
    }
    tmp.append(" ...\n");

    cnt.append(tmp);
    tmp.clear();
  }

  return cnt;
}

void Table::clear() {
  m_table_col_names.clear();
  m_table_info.clear();
  m_table_storage.clear();
  m_meta_storage.clear();
  m_meta_info.clear();
}

bool Table::empty() const {
  return m_table_col_names.empty() && m_table_info.empty() &&
         m_table_storage.empty() && m_meta_storage.empty() &&
         m_meta_info.empty();
}

std::size_t Table::size() const {
  this->self_check();
  return m_table_storage.size();
}

void Table::self_check() const {
  gaa_assert(m_table_storage.size() == m_table_info.size(),
             "Internal storage is broken");
  gaa_assert(m_table_storage.size() == m_table_col_names.size(),
             "Internal storage is broken");
  gaa_assert(m_meta_storage.size() == m_meta_info.size(),
             "Internal storage is broken");
}

std::ptrdiff_t Table::column_of(column_name_type const &name) const {
  auto dest =
      std::find(m_table_col_names.begin(), m_table_col_names.end(), name);
  gaa_assert(dest != m_table_col_names.end(), "column name not found in table");
  std::ptrdiff_t idx = std::distance(m_table_col_names.begin(), dest);
  return idx;
}

std::size_t Table::column_size() const { return this->m_table_storage.size(); }

bool Table::has_column(column_name_type const &name) const {
  auto dest =
      std::find(m_table_col_names.begin(), m_table_col_names.end(), name);
  return dest != m_table_col_names.end();
}

Table_row_view Table::row(index_type const &idx) const {
  return Row_view(*this, idx);
}

Table_row_view Table::row(column_name_type const &name) const {
  return this->row(column_of(name));
}

Table_row_ref Table::row(index_type const &idx) { return Row_ref(*this, idx); }

Table_row_ref Table::row(column_name_type const &name) {
  return this->row(column_of(name));
}

Table_Storage_Flag table_storage_flag_of(std::string const &str) {
  static const boost::regex tab_double(
      R"(^[+-]?([0-9]+)\.([0-9]+)([Ee][+-]?[0-9]+)?$)");
  static const boost::regex tab_int(R"(^[+-]?([0-9]+)$)");
  static const boost::regex tab_bool(
      R"(^([Tt]rue)|(TRUE)|([Ff]alse)|(FALSE)$)");
  static const boost::regex tab_string(R"(^[\w\/\.]+$)");

  if (boost::regex_match(str, tab_double)) {
    return Tab_Double;
  } else if (boost::regex_match(str, tab_int)) {
    return Tab_Integer;
  } else if (boost::regex_match(str, tab_bool)) {
    return Tab_Bool;
  } else if (boost::regex_match(str, tab_string)) {
    return Tab_String;
  } else {
    return Tab_Unsupported;
  }
}

bool Table::meta_has(meta_info_key_type const &key) const {
  return m_meta_storage.contains(key);
}
Table::mate_storage_type const &Table::meta() const {
  return this->m_meta_storage;
}
Table::mate_storage_type &Table::meta() { return m_meta_storage; }

Table::table_storage_type const &Table::columns() const {
  return m_table_storage;
}
Table::table_storage_type &Table::columns() { return m_table_storage; }
Table::column_name_storage_type const &Table::column_names() const {
  return m_table_col_names;
}
Table::column_name_storage_type &Table::column_names() {
  return m_table_col_names;
}
Table::table_storage_info_type const &Table::column_infos() const {
  return m_table_info;
}
Table::table_storage_info_type &Table::column_infos() { return m_table_info; }

} // namespace gaa