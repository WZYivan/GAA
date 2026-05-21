#include <fstream>
#include <print>
#include <string>

#include <boost/algorithm/string.hpp>

#include <gaa/gnss/space/sv_pos_from_broadcast.hpp>
#include <gaa/io/csv.hpp>
#include <gaa/io/rinex3/nav.hpp>

int main() {
  std::string fname{"/home/azusa/file/project/GAA/data/gths135a.18f"};
  std::ifstream ifs{fname, std::ios::in};

  gaa::Rinex3_nav rinex3 = gaa::read_rinex3_nav(fname);
  std::cout << rinex3.glimpse();

  auto row0 = rinex3.row(0);
  auto param = gaa::Param_sv_pos_from_broadcast::from_table_row(
      row0, row0.at<gaa::Tab_double>("t_oe"));

  auto [x, y, z, _e] = gaa::sv_pos_from_broadcast(param);
  std::println("x = {}, y = {}, z = {}", x, y, z);

  gaa::write_csv("nav.csv", rinex3.table(), gaa::kw{}.stream_precision(3));

  return EXIT_SUCCESS;
}