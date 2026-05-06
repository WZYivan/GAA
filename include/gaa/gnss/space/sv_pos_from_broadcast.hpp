#pragma once

#include <gaa/core/time.hpp>
#include <gaa/geodesy/ellipsoids.hpp>
#include <gaa/gnss/rinex.hpp>
#include <gaa/gnss/space/sv_orbit.hpp>

namespace gaa {
#if !GAA_MSVC && (0)
// template <class Ephemeris>
inline std::tuple<double, double, double>
sv_pos_from_broadcast(rinex::v302::Nav_ephemeris_bds const &eph, double t)
// requires requires {
//     eph.sqrtA;
// }
{
  double GM = cgcs2000.gravity().GM(), omegaE = cgcs2000.gravity().omega();
  double sqrtA = eph.sqrtA;
  double n0 = sv_mean_angular_velocity(GM, sqrtA);

  double e = eph.e_Eccentricity, toe = eph.Toe, tk = t - toe;

  double M0 = eph.M0, Mk = M0 + n0 * tk, Ek = sv_eccentric_anomaly(Mk, e),
         fk = sv_true_anomaly(Ek, e);

  double omega = eph.omega, Phi = omega + fk, i0 = eph.i0, IDOT = eph.IDOT,
         r = sv_geocentric_distance(std::pow(sqrtA, 2), e, fk);

  Perturb_correction corr = sv_perturb_corr(Phi, eph.Cuc, eph.Cus, eph.Crc,
                                            eph.Crs, eph.Cic, eph.Cis);
  auto [uk, rk, ik] = sv_apply_perturb_correction(corr, Phi, r, i0);

  auto [xk, yk, zk] = sv_orbit_coordinate(uk, rk);

  double L = sv_longitude_argument(eph.OMEGA0, eph.OMEGA_DOT, omegaE, t, toe);
  auto earth_coord = sv_instance_earth_coordinate(xk, yk, ik, L);

  return earth_coord;
}
#endif
} // namespace gaa