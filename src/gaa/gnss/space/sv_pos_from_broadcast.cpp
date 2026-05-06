#include <gaa/gnss/space/sv_pos_from_broadcast.hpp>

namespace gaa {

std::tuple<double, double, double>
sv_pos_from_broadcast(Data_frame const &df, Data_frame_idx row, double t) {
  gaa_assert(df.has_column("sqrtA"));
  gaa_assert(df.has_column("e"));
  gaa_assert(df.has_column("t_oe"));
  gaa_assert(df.has_column("M_0"));
  gaa_assert(df.has_column("omega"));
  gaa_assert(df.has_column("i_0"));
  gaa_assert(df.has_column("IDOT"));
  gaa_assert(df.has_column("C_us"));
  gaa_assert(df.has_column("C_uc"));
  gaa_assert(df.has_column("C_is"));
  gaa_assert(df.has_column("C_ic"));
  gaa_assert(df.has_column("C_rc"));
  gaa_assert(df.has_column("C_rs"));
  gaa_assert(df.has_column("Omega_0"));
  gaa_assert(df.has_column("OmegaDot"));

  double GM = cgcs2000.gravity().GM(), omegaE = cgcs2000.gravity().omega();
  double sqrtA = df_at<double>(df, row, "sqrtA");
  double n0 = sv_mean_angular_velocity(GM, sqrtA);

  double e = df_at<double>(df, row, "e"), toe = df_at<double>(df, row, "t_oe"),
         tk = t - toe;

  double M0 = df_at<double>(df, row, "M_0"), Mk = M0 + n0 * tk,
         Ek = sv_eccentric_anomaly(Mk, e), fk = sv_true_anomaly(Ek, e);

  double omega = df_at<double>(df, row, "omega"), Phi = omega + fk,
         i0 = df_at<double>(df, row, "i_0"),
         IDOT = df_at<double>(df, row, "IDOT"),
         r = sv_geocentric_distance(std::pow(sqrtA, 2), e, fk);

  Perturb_correction corr = sv_perturb_corr(
      Phi, df_at<double>(df, row, "C_uc"), df_at<double>(df, row, "C_us"),
      df_at<double>(df, row, "C_rc"), df_at<double>(df, row, "C_rs"),
      df_at<double>(df, row, "C_ic"), df_at<double>(df, row, "C_is"));
  auto [uk, rk, ik] = sv_apply_perturb_correction(corr, Phi, r, i0);

  auto [xk, yk, zk] = sv_orbit_coordinate(uk, rk);

  double L =
      sv_longitude_argument(df_at<double>(df, row, "Omega_0"),
                            df_at<double>(df, row, "OmegaDot"), omegaE, t, toe);
  auto coord = sv_instance_earth_coordinate(xk, yk, ik, L);

  return coord;
}
} // namespace gaa