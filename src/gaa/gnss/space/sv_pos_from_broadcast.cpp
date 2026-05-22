#include <gaa/gnss/space/sv_pos_from_broadcast.hpp>

namespace gaa {

Ecef_coordinate sv_pos_from_broadcast(double sqrtA, double e, double t_oe,
                                      double M_0, double omega, double i_0,
                                      double IDOT [[maybe_unused]], double C_us,
                                      double C_uc, double C_is, double C_ic,
                                      double C_rc, double C_rs, double Omega_0,
                                      double OmegaDot, double t) {
  double GM = cgcs2000.gravity().GM(), omegaE = cgcs2000.gravity().omega();
  double n0 = sv_mean_angular_velocity(GM, sqrtA);

  double tk = t - t_oe;

  double Mk = M_0 + n0 * tk, Ek = sv_eccentric_anomaly(Mk, e),
         fk = sv_true_anomaly(Ek, e);

  double Phi = omega + fk,
         r = sv_geocentric_distance(std::pow(sqrtA, 2), e, fk);

  Perturb_correction corr =
      sv_perturb_corr(Phi, C_uc, C_us, C_rc, C_rs, C_ic, C_is);
  auto [uk, rk, ik] = sv_apply_perturb_correction(corr, Phi, r, i_0);

  auto [xk, yk, zk] = sv_orbit_coordinate(uk, rk);

  double L = sv_longitude_argument(Omega_0, OmegaDot, omegaE, t, t_oe);
  auto [x, y, z] = sv_instance_earth_coordinate(xk, yk, ik, L);

  return Ecef_coordinate(x, y, z);
}
Ecef_coordinate
sv_pos_from_broadcast(Param_sv_pos_from_broadcast const &param) {
  return sv_pos_from_broadcast(
      param.sqrtA, param.e, param.t_oe, param.M_0, param.omega, param.i_0,
      param.IDOT, param.C_us, param.C_uc, param.C_is, param.C_ic, param.C_rc,
      param.C_rs, param.Omega_0, param.OmegaDot, param.t);
}

Param_sv_pos_from_broadcast
Param_sv_pos_from_broadcast::from_table_row(Table_row_ref row, double t) {
  return Param_sv_pos_from_broadcast{
      .sqrtA = row.at<Tab_double>("sqrtA"),
      .e = row.at<Tab_double>("e"),
      .t_oe = row.at<Tab_double>("t_oe"),
      .M_0 = row.at<Tab_double>("M_0"),
      .omega = row.at<Tab_double>("omega"),
      .i_0 = row.at<Tab_double>("i_0"),
      .IDOT = row.at<Tab_double>("IDOT"),
      .C_us = row.at<Tab_double>("C_us"),
      .C_uc = row.at<Tab_double>("C_uc"),
      .C_is = row.at<Tab_double>("C_is"),
      .C_ic = row.at<Tab_double>("C_ic"),
      .C_rc = row.at<Tab_double>("C_rc"),
      .C_rs = row.at<Tab_double>("C_rs"),
      .Omega_0 = row.at<Tab_double>("Omega_0"),
      .OmegaDot = row.at<Tab_double>("OmegaDot"),
      .t = t,
  };
}

Param_sv_pos_from_broadcast
Param_sv_pos_from_broadcast::from_table_row(Table_row_view row, double t) {
  return Param_sv_pos_from_broadcast{
      .sqrtA = row.at<Tab_double>("sqrtA"),
      .e = row.at<Tab_double>("e"),
      .t_oe = row.at<Tab_double>("t_oe"),
      .M_0 = row.at<Tab_double>("M_0"),
      .omega = row.at<Tab_double>("omega"),
      .i_0 = row.at<Tab_double>("i_0"),
      .IDOT = row.at<Tab_double>("IDOT"),
      .C_us = row.at<Tab_double>("C_us"),
      .C_uc = row.at<Tab_double>("C_uc"),
      .C_is = row.at<Tab_double>("C_is"),
      .C_ic = row.at<Tab_double>("C_ic"),
      .C_rc = row.at<Tab_double>("C_rc"),
      .C_rs = row.at<Tab_double>("C_rs"),
      .Omega_0 = row.at<Tab_double>("Omega_0"),
      .OmegaDot = row.at<Tab_double>("OmegaDot"),
      .t = t,
  };
}
} // namespace gaa