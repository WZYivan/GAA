#include <boost/regex.hpp>

#include <gaa/core/table.hpp>

namespace gaa {
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
} // namespace gaa