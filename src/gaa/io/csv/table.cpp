#include <charconv>
#include <filesystem>
#include <fstream>
#include <ranges>

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

#include <gaa/io/csv/table.hpp>

namespace fs = std::filesystem;

namespace gaa {
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

    for (std::size_t i = 0; i != seq.size(); ++i) {
      cnts.at(i).emplace_back(std::move(seq.at(i)));
    }
  }

  if (ignore_first_line) {
    gaa_assert(names.empty(),
               "Internal error, ignore first line but names is not empty");
    auto cols = cnts.size();
    for (std::size_t i = 0; i != cols; ++i) {
      names.emplace_back(std::format("COL_{:d}", i));
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
        gaa_assert(ec == std::errc{},
                   "error occurs in parsing `{:s}` as Tab_double", str);
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
        gaa_assert(ec == std::errc{},
                   "error occurs in parsing `{:s}` as Tab_int", str);
        col.emplace_back(val);
      }
      tab.push_back(std::move(col), name);
      break;
    }
    case Tab_Bool: {
      Table_column<Tab_bool> col;
      col.reserve(cnt.size());
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
      gaa_fail("unreachable default case");
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
        flags.emplace_back(table_storage_flag_of(str));
      }
    } else {
      gaa_assert(seq.size() == flags.size(),
                 "given flags mismatch actual components count");
    }

    for (std::size_t i = 0; i != seq.size(); ++i) {
      cnts.at(i).emplace_back(std::move(seq.at(i)));
    }
  }

  if (ignore_first_line) {
    gaa_assert(names.empty(),
               "Internal error, ignore first line but names is not empty");
    auto cols = cnts.size();
    for (std::size_t i = 0; i != cols; ++i) {
      names.emplace_back(std::format("COL_{:d}", i));
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
        gaa_assert(ec == std::errc{},
                   "error occurs in parsing `{:s}` as Tab_double", str);
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
        gaa_assert(ec == std::errc{},
                   "error occurs in parsing `{:s}` as Tab_int", str);
        col.emplace_back(val);
      }

      tab.push_back(std::move(col), name);
      break;
    }
    case Tab_Bool: {
      Table_column<Tab_bool> col;
      col.reserve(cnt.size());
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
    case Tab_Lat: {
      Table_column<Tab_lat> col;
      col.reserve(cnt.size());
      double dbl;
      for (auto const &str : cnt) {
        auto [ptr, ec] =
            std::from_chars(str.data(), str.data() + str.size() - 4, dbl);
        gaa_assert(ec == std::errc{},
                   "error occurs in parsing `{:s}` as Tab_lat", str);
        col.emplace_back(deg2rad(dbl));
      }

      tab.push_back(std::move(col), name);
      break;
    }
    case Tab_Lon: {
      Table_column<Tab_lon> col;
      col.reserve(cnt.size());
      double dbl;
      for (auto const &str : cnt) {
        auto [ptr, ec] =
            std::from_chars(str.data(), str.data() + str.size() - 4, dbl);
        gaa_assert(ec == std::errc{},
                   "error occurs in parsing `{:s}` as Tab_lon", str);
        col.emplace_back(deg2rad(dbl));
      }

      tab.push_back(std::move(col), name);
      break;
    }
    case Tab_Rad: {
      Table_column<Tab_rad> col;
      col.reserve(cnt.size());
      double dbl;
      for (auto const &str : cnt) {
        auto [ptr, ec] =
            std::from_chars(str.data(), str.data() + str.size() - 4, dbl);
        gaa_assert(ec == std::errc{},
                   "error occurs in parsing `{:s}` as Tab_rad", str);
        col.emplace_back(deg2rad(dbl));
      }

      tab.push_back(std::move(col), name);
      break;
    }
    default:
      gaa_fail("unreachable default case");
    }
  }
  return tab;
}

void write_csv(std::string const &fname, Table const &table, kwargs args) {
  fs::path fpath{fname};
  if (fpath.has_parent_path() && !fs::exists(fpath.parent_path())) {
    fs::create_directories(fpath.parent_path());
  }
  std::ofstream ofs{fpath, std::ios::out};
  gaa_assert(ofs, "bad ofstream using \'{}\'", fname);
  write_csv(ofs, table, args);
}

void write_csv(std::ostream &os, Table const &table, kwargs args) {
  GAA_ARG_OR(args, stream_precision, 0);

  auto const &col_names = table.column_names();
  auto const &col_infos = table.column_infos();
  auto const &col_storages = table.columns();
  size_t num_cols = col_names.size();
  if (num_cols == 0)
    return;

  size_t num_rows = 0;
  auto get_size = [&](size_t col) -> size_t {
    size_t sz = 0;
    visit_table_storage(col_infos.at(col), col_storages.at(col),
                        [&sz](auto const &vec) { sz = vec.size(); });
    return sz;
  };
  num_rows = get_size(0);

  std::vector<std::vector<std::string>> col_strs(num_cols);
  auto storage2str = [stream_precision]<class T>(T const &val) {
    if constexpr (std::is_same_v<T, UTC_Identifier> ||
                  std::is_same_v<T, Satellite_System>) {
      return enum2str(val);
    } else if constexpr (std::same_as<Latitude, std::decay_t<T>>) {
      return std::format("{:.{}f}_lat", deg(val), stream_precision);
    } else if constexpr (std::same_as<Longitude, std::decay_t<T>>) {
      return std::format("{:.{}f}_lon", deg(val), stream_precision);
    } else if constexpr (std::same_as<Radian, std::decay_t<T>>) {
      return std::format("{:.{}f}_rad", deg(val), stream_precision);
    } else if constexpr (std::is_floating_point_v<T>) {
      return std::format("{:.{}f}", val, stream_precision);
    } else {
      return std::format("{}", val);
    }
  };

  for (size_t c = 0; c < num_cols; ++c) {
    auto &str_vec = col_strs[c];
    visit_table_storage(col_infos.at(c), col_storages.at(c),
                        [&](auto const &vec) {
                          str_vec.reserve(vec.size());
                          for (auto const &v : vec) {
                            str_vec.push_back(storage2str(v));
                          }
                        });
  }

  for (size_t c = 0; c < num_cols; ++c) {
    os << col_names[c];
    if (c != num_cols - 1)
      os << ',';
  }
  os << '\n';

  for (size_t r = 0; r < num_rows; ++r) {
    for (size_t c = 0; c < num_cols; ++c) {
      os << col_strs[c][r];
      if (c != num_cols - 1)
        os << ',';
    }
    os << '\n';
  }
}
} // namespace gaa