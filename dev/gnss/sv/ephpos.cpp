#include <gaa/gnss/space/sv_pos_from_broadcast.hpp>
#include <gaa/io/csv.hpp>
#include <gaa/io/rinex3/nav.hpp>
#include <sstream>

int main() {
  auto nav =
      gaa::read_rinex3_nav("/home/azusa/file/project/GAA/data/gths135a.18f");
  auto const &access = nav.table();
  auto const &sat_prn = access.at<gaa::Tab_int>("sat prn");
  auto const &sat_sys = access.at<gaa::Tab_sat_sys>("sat sys");
  std::size_t rows = access.row_size();

  gaa::Table coord;
  auto &ecef_x = coord.push_for_insert<gaa::Tab_double>("Ecef X");
  auto &ecef_y = coord.push_for_insert<gaa::Tab_double>("Ecef Y");
  auto &ecef_z = coord.push_for_insert<gaa::Tab_double>("Ecef Z");
  auto &geo_lat = coord.push_for_insert<gaa::Tab_lat>("Geo Lat");
  auto &geo_lon = coord.push_for_insert<gaa::Tab_lon>("Geo Lon");
  auto &geo_h = coord.push_for_insert<gaa::Tab_double>("Geo H");

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

    ecef_x.push_back(x);
    ecef_y.push_back(y);
    ecef_z.push_back(z);
    geo_lat.push_back(B);
    geo_lon.push_back(L);
    geo_h.push_back(H);
  }

  std::cout << "Coords:\n" << coord.glimpse() << "\n";

  std::stringstream oss{};
  gaa::write_csv(oss, coord, gaa::mkarg::stream_precision(5));
  auto coord2 = gaa::read_csv_auto(oss);
  std::cout << "Coords reading from oss:\n" << coord2.glimpse() << "\n";

  std::cout << "CSV in oss:\n" << oss.str() << "\n";
}