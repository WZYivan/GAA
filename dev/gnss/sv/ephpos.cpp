#include <gaa/gnss/space/sv_pos_from_broadcast.hpp>
#include <print>

int main() {
#if !GAA_MSVC && (0)
  gaa::rinex::v302::Nav_ephemeris_bds eph;

  eph.sqrtA = 0.515365263176e04;
  eph.e_Eccentricity = 0.678421219345e-02;
  eph.M0 = -0.290282040486e00;
  eph.Toe = 0.72e4;
  eph.year = std::chrono::year(1997);
  eph.month = std::chrono::month(11);
  eph.day = std::chrono::day(9);
  eph.hour = std::chrono::hours(2);
  eph.minute = std::chrono::minutes(0);
  eph.second = std::chrono::seconds(0);
  eph.omega = -0.258419417299e01;
  eph.OMEGA0 = -0.137835982556e01;
  eph.OMEGA_DOT = -0.819426989566e-08;
  eph.i0 = 0.958512160302e00;
  eph.IDOT = -0.253939149013e-09;
  eph.Cus = 0.912137329578e-05;
  eph.Cuc = 0.189989805222e-06;
  eph.Crs = 0.406250000000e01;
  eph.Crc = 0.201875000000e03;
  eph.Cis = -0.949949026108e-07;
  eph.Cic = 0.130385160446e-07;

  auto [x, y, z] = gaa::sv_pos_from_broadcast(eph, 7200);
#endif

  gaa::Data_frame df;
  df.read("data/gths135a.18f.hmdf.csv", hmdf::io_format::csv2,
          {.skip_first_line = true});
  auto [x, y, z] =
      gaa::sv_pos_from_broadcast(df, 0, gaa::df_at<double>(df, 0, "t_oe"));
  std::println("x = {}, y = {}, z = {}", x, y, z);

  // std::vector<unsigned long> idx_col1 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  // std::vector<int> int_col1 = {1, 2, -3, -4, 5, 6, 7, 8, 9, -10};
  // std::vector<double> dbl_col1 = {0.01, 0.02, 0.03, 0.03, 0.05,
  //                                 0.06, 0.03, 0.08, 0.09, 0.03};

  // gaa::Data_frame ul_df1;
  // ul_df1.load_index(std::move(idx_col1));
  // ul_df1.load_column("dbl_col", std::move(dbl_col1));
  // ul_df1.load_column("integers", std::move(int_col1));

  // std::fstream ofs{"out.csv2", std::ios::out};
  // ul_df1.write<int, double>("out.csv2", hmdf::io_format::csv2);
}