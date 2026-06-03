#include <filesystem>
#include <fstream>

#include <boost/algorithm/string.hpp>

#include <gaa/core/literal.hpp>
#include <gaa/io/csv/frame.hpp>

namespace fs = std::filesystem;

namespace gaa {
Frame _read_csv_to_frame(std::vector<std::vector<std::string>> elements,
                         std::span<Literal_Type const> literals,
                         bool has_head) {
  {
    std::size_t col_size = elements.begin()->size();
    for (std::size_t i = 0; i != elements.size(); ++i) {
      gaa_assert(elements.at(i).size() == col_size,
                 "col {} size {} mismatch, expected {}", i,
                 elements.at(i).size(), col_size);
    }
    gaa_assert(literals.size() == col_size,
               "given size of literals hint {} mismatch with given content {}",
               literals.size(), col_size);
  }

  Frame df(elements.size());
  std::vector<std::string> head =
      has_head ? elements.front()
               : std::views::iota(0uz, elements.begin()->size()) |
                     std::views::transform([](std::size_t i) -> std::string {
                       return std::format("COL_{}", i);
                     }) |
                     std::ranges::to<std::vector<std::string>>();

  for (std::size_t i = 0; i != literals.size(); ++i) {
    df.new_col(head.at(i), literals[i]);
  }

  {
    std::size_t row_it = has_head ? 1 : 0;
    std::size_t rows = elements.size();
    std::size_t cols = literals.size();
    for (; row_it != rows; ++row_it) {
      std::vector<std::string> const &row_elems = elements.at(row_it);
      for (std::size_t col_it = 0; col_it != cols; ++col_it) {
        df.col(col_it).assign_at(
            row_it, literal_cast(row_elems.at(col_it), literals[col_it]));
      }
    }
  }

  return df;
}

Frame read_csv_to_frame(std::string const &fname,
                        std::span<Literal_Type const> literals, kwargs args) {
  fs::path fpath{fname};
  gaa_assert(fs::exists(fpath), "file {} not exits", fpath.string());
  std::ifstream ifs{fname, std::ios::in};
  return read_csv_to_frame(ifs, literals, args);
}

Frame read_csv_to_frame(std::istream &is,
                        std::span<Literal_Type const> literals, kwargs args) {
  gaa_assert(is.good(), "bad stream");

  GAA_ARG_OR(args, separator, ',');
  GAA_ARG_OR(args, ignore_first_line, false);

  std::vector<std::vector<std::string>> elems;
  std::string line;

  if (ignore_first_line) {
    /// skip first non-empty line
    do {
      std::getline(is, line);
    } while (line.empty());
  }

  while (std::getline(is, line)) {
    if (line.empty()) {
      break;
    }

    elems.emplace_back(line | std::views::split(separator) |
                       std::ranges::to<std::vector<std::string>>());
    for (auto &str : elems.back()) {
      boost::trim(str);
    }
  }

  return _read_csv_to_frame(elems, literals, ignore_first_line);
}

Frame read_csv_to_frame(std::string const &fname, kwargs args) {
  fs::path fpath{fname};
  gaa_assert(fs::exists(fpath), "file {} not exits", fpath.string());
  std::ifstream ifs{fname, std::ios::in};
  return read_csv_to_frame(ifs, args);
}

Frame read_csv_to_frame(std::istream &is, kwargs args) {
  gaa_assert(is.good(), "bad stream");

  GAA_ARG_OR(args, separator, ',');
  GAA_ARG_OR(args, ignore_first_line, false);

  std::vector<std::vector<std::string>> elems;
  std::string line;

  if (ignore_first_line) {
    /// skip first non-empty line
    do {
      std::getline(is, line);
    } while (line.empty());
  }

  while (std::getline(is, line)) {
    if (line.empty()) {
      break;
    }

    elems.emplace_back(line | std::views::split(separator) |
                       std::ranges::to<std::vector<std::string>>());
    for (auto &str : elems.back()) {
      boost::trim(str);
    }
  }

  std::vector<Literal_Type> literals =
      (ignore_first_line ? elems.at(0) : elems.at(1)) |
      std::views::transform(literal_detect) |
      std::ranges::to<std::vector<Literal_Type>>();

  return _read_csv_to_frame(elems, literals, ignore_first_line);
}

void write_csv(std::string const &fname, Frame const &df, kwargs args) {
  fs::path fpath{fname};
  std::ofstream ofs{fname, std::ios::in};
  gaa_assert(fs::exists(fpath), "create file {} fail", fpath.string());
  return write_csv(ofs, df, args);
}

void write_csv(std::ostream &os, Frame const &df, kwargs args) {
  gaa_assert(os.good(), "bad stream");

  GAA_ARG_OR(args, separator, ',');
  GAA_ARG_OR(args, ignore_first_line, false);

  std::vector<std::string> rows;
  rows.resize(df.rows());

  for (auto col : df.indices()) {
    Any const &col_vec = df.col(col);
    std::size_t row = 0;
    col_vec.for_each([&rows, &row, col](Any const &elem) -> void {
      std::string &row_cnt = rows.at(row);
      if (col != 0) {
        row_cnt.append(", ");
      }
      row_cnt.append(elem.format());
      ++row;
    });
  }

  if (!ignore_first_line) {
    auto head = df.names() | std::views::join_with(separator) |
                std::ranges::to<std::string>();

    std::println(os, "{}", head);
  }
  for (auto const &row : rows) {
    std::println(os, "{}", row);
  }
}
} // namespace gaa