#include <gaa/gnss/space/sv_pos_from_broadcast.hpp>
#include <gaa/io/rinex3/nav.hpp>
#include <print>

int main() {
  auto nav =
      gaa::read_rinex3_nav("/home/azusa/file/project/GAA/data/gths135a.18f");
  auto const &access = nav.table();
  auto const &sat_prn = access.at<gaa::Tab_int>("sat prn");
  auto const &sat_sys = access.at<gaa::Tab_sat_sys>("sat sys");
  std::size_t rows = sat_prn.size();

  for (std::size_t r = 0; r != rows; ++r) {
    gaa::Table_row_view row = access.row(r);
    auto param = gaa::Param_sv_pos_from_broadcast::from_table_row(
        row, row.at<gaa::Tab_double>("t_oe"));
    auto src = gaa::sv_pos_from_broadcast(param);
    src.ellipsoid = gaa::ellipsoid_of(sat_sys.at(r));
    auto [x, y, z, _src_e] = src;
    auto geo = src >> gaa::geodetic_cast;
    auto [B, L, H, _geo_e] = geo;
    auto prn = sat_prn.at(r);
    auto sys = sat_sys.at(r);
    gaa_assert(std::abs(gaa::rad(B)) < std::numbers::pi / 2);
    std::println("{} {}: x = {:.5f}, y = {:.5f}, z = {:.5f}; L = {:.5f}, B = "
                 "{:.5f}, H = {:.5f}",
                 gaa::enum2str(sys), prn, x, y, z, gaa::rad2deg(gaa::rad(L)),
                 gaa::rad2deg(gaa::rad(B)), H);
  }
}