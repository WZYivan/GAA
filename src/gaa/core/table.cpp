#include <charconv>
#include <filesystem>
#include <fstream>
#include <ranges>

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

#include <gaa/core/table.hpp>

namespace fs = std::filesystem;

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

Table read_csv(std::string const &fname,
               std::vector<Table_Storage_Flag> const &flags, kwargs args) {
  fs::path fpath{fname};
  gaa_assert(fs::exists(fpath));
  std::ifstream ifs{fpath, std::ios::in};
  return read_csv(ifs, flags, args);
}

Table read_csv(std::istream &is, std::vector<Table_Storage_Flag> const &flags,
               kwargs args) {
  gaa_assert(!flags.empty());

  GAA_ARG_OR(args, separator, ',');
  GAA_ARG_OR(args, ignore_first_line, false);

  std::string line{};

  using Str_seq = std::vector<std::string>;
  Str_seq names;
  if (!ignore_first_line) {
    std::getline(is, line);
    boost::split(names, line,
                 [separator](char c) -> bool { return c == separator; });
    for (auto &str : names) {
      boost::trim(str);
    }
  }

  std::vector<Str_seq> cnts;
  if (!ignore_first_line) {
    cnts.resize(names.size());
  }

  while (std::getline(is, line)) {
    if (line.empty()) {
      break;
    }

    Str_seq seq{};
    boost::split(seq, line,
                 [separator](char c) -> bool { return c == separator; });

    if (ignore_first_line && cnts.size() == 0) {
      cnts.resize(seq.size());
      continue;
    }

    gaa_assert(seq.size() == flags.size(),
               "given flags mismatch actual components count");

    for (int i = 0; i != seq.size(); ++i) {
      cnts.at(i).emplace_back(std::move(seq.at(i)));
    }
  }

  if (ignore_first_line) {
    gaa_assert(names.empty(),
               "Internal error, ignore first line but names is not empty");
    auto cols = cnts.size();
    for (auto i = 0; i != cols; ++i) {
      names.emplace_back(std::move(std::format("COL_{:d}", i)));
    }
  }

  Table tab;
  for (auto const &[name, cnt, flag] : std::views::zip(names, cnts, flags)) {
    switch (flag) {
    case Tab_String: {
      tab.push_back(std::move(cnt), name);
      break;
    }
    case Tab_Double: {
      Table_column<Tab_double> col;
      col.reserve(cnt.size());
      double dbl;
      for (auto const &str : cnt) {
        auto [ptr, ec] =
            std::from_chars(str.data(), str.data() + str.size(), dbl);
        gaa_assert(
            ec == std::errc{},
            std::format("error occurs in parsing `{:s}` as Tab_double", str));
        col.emplace_back(dbl);
      }
      tab.push_back(std::move(col), name);
      break;
    }
    case Tab_Integer: {
      Table_column<Tab_int> col;
      col.reserve(cnt.size());
      int val;
      for (auto const &str : cnt) {
        auto [ptr, ec] =
            std::from_chars(str.data(), str.data() + str.size(), val);
        gaa_assert(
            ec == std::errc{},
            std::format("error occurs in parsing `{:s}` as Tab_int", str));
        col.emplace_back(val);
      }
      tab.push_back(std::move(col), name);
      break;
    }
    case Tab_Bool: {
      Table_column<Tab_bool> col;
      col.reserve(cnt.size());
      int val;
      for (auto &str : cnt) {
        for (auto &c : str) {
          c = std::tolower(c);
        }
        if (str == "true") {
          col.emplace_back(true);
        } else {
          col.emplace_back(false);
        }
      }

      tab.push_back(std::move(col), name);
      break;
    }
    default:
      gaa_assert(false, "unreachable default case");
    }
  }
  return tab;
}

Table read_csv_auto(std::string const &fname, kwargs args) {
  fs::path fpath{fname};
  gaa_assert(fs::exists(fpath));
  std::ifstream ifs{fpath, std::ios::in};
  return read_csv_auto(ifs, args);
}

Table_Storage_Flag _str2Table_Storage_Flag(std::string const &str) {

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

Table read_csv_auto(std::istream &is, kwargs args) {
  GAA_ARG_OR(args, separator, ',');
  GAA_ARG_OR(args, ignore_first_line, false);

  std::string line{};

  using Str_seq = std::vector<std::string>;
  Str_seq names;
  if (!ignore_first_line) {
    std::getline(is, line);
    boost::split(names, line,
                 [separator](char c) -> bool { return c == separator; });
    for (auto &str : names) {
      boost::trim(str);
    }
  }

  std::vector<Str_seq> cnts;
  if (!ignore_first_line) {
    cnts.resize(names.size());
  }

  std::vector<Table_Storage_Flag> flags;
  bool flag_init = false;
  while (std::getline(is, line)) {
    if (line.empty()) {
      break;
    }

    Str_seq seq{};
    boost::split(seq, line,
                 [separator](char c) -> bool { return c == separator; });

    if (ignore_first_line && cnts.size() == 0) {
      cnts.resize(seq.size());
      continue;
    }

    if (!flag_init) {
      for (auto &str : seq) {
        boost::trim(str);
        flags.emplace_back(_str2Table_Storage_Flag(str));
      }
    } else {
      gaa_assert(seq.size() == flags.size(),
                 "given flags mismatch actual components count");
    }

    for (int i = 0; i != seq.size(); ++i) {
      cnts.at(i).emplace_back(std::move(seq.at(i)));
    }
  }

  if (ignore_first_line) {
    gaa_assert(names.empty(),
               "Internal error, ignore first line but names is not empty");
    auto cols = cnts.size();
    for (auto i = 0; i != cols; ++i) {
      names.emplace_back(std::move(std::format("COL_{:d}", i)));
    }
  }

  Table tab;
  for (auto const &[name, cnt, flag] : std::views::zip(names, cnts, flags)) {
    switch (flag) {
    case Tab_String: {
      tab.push_back(std::move(cnt), name);
      break;
    }
    case Tab_Double: {
      Table_column<Tab_double> col;
      col.reserve(cnt.size());
      double dbl;
      for (auto const &str : cnt) {
        auto [ptr, ec] =
            std::from_chars(str.data(), str.data() + str.size(), dbl);
        gaa_assert(
            ec == std::errc{},
            std::format("error occurs in parsing `{:s}` as Tab_double", str));
        col.emplace_back(dbl);
      }

      tab.push_back(std::move(col), name);
      break;
    }
    case Tab_Integer: {
      Table_column<Tab_int> col;
      col.reserve(cnt.size());
      int val;
      for (auto const &str : cnt) {
        auto [ptr, ec] =
            std::from_chars(str.data(), str.data() + str.size(), val);
        gaa_assert(
            ec == std::errc{},
            std::format("error occurs in parsing `{:s}` as Tab_int", str));
        col.emplace_back(val);
      }

      tab.push_back(std::move(col), name);
      break;
    }
    case Tab_Bool: {
      Table_column<Tab_bool> col;
      col.reserve(cnt.size());
      int val;
      for (auto &str : cnt) {
        for (auto &c : str) {
          c = std::tolower(c);
        }
        if (str == "true") {
          col.emplace_back(true);
        } else {
          col.emplace_back(false);
        }
      }

      tab.push_back(std::move(col), name);
      break;
    }
    default:
      gaa_assert(false, "unreachable default case");
    }
  }
  return tab;
}

} // namespace gaa