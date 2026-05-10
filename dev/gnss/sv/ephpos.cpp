#include <gaa/core/io/csv.hpp>
#include <gaa/core/table.hpp>
#include <gaa/gnss/space/sv_pos_from_broadcast.hpp>
#include <print>

int main() {
  gaa::Table tab = gaa::read_csv_auto(
      "/home/azusa/file/project/GAA/data/gths135a.18f.dat.csv");
  auto row0 = tab.row(0);
  auto [x2, y2, z2] = gaa::sv_pos_from_broadcast(
      gaa::Param_sv_pos_from_broadcast::from_table_row(
          row0, row0.at<gaa::Tab_double>("t_oe")));
  std::println("x2 = {}, y2 = {}, z2 = {}", x2, y2, z2);
}