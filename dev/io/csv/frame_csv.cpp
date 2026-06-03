#include <gaa/io/csv.hpp>
#include <iostream>
#include <print>

int main() {
  auto csv = gaa::read_csv_to_frame(
      "/home/azusa/file/project/GAA/data/cass.dat",
      std::array{gaa::Literal_Type::String, gaa::Literal_Type::String,
                 gaa::Literal_Type::Digital, gaa::Literal_Type::Digital,
                 gaa::Literal_Type::Digital},
      gaa::mkarg::ignore_first_line(false));

  gaa::write_csv(std::cout, csv, gaa::mkarg::ignore_first_line(true));
}