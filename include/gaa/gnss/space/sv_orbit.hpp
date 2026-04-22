#pragma once

#include <gaa/core/kw.hpp>
#include <gaa/gnss/rinex.hpp>

namespace gaa
{
    extern double sv_mean_angular_velocity(double GM, double sqrtA);
    extern double sv_eccentric_anomaly(double mean_anomaly, double e, kwargs args = {});
    extern double sv_true_anomaly(double eccentric_anomaly, double e);
    extern double sv_latitude_argument(double true_anomaly, double omega);
    extern double sv_longitude_argument(double OMEGA0, double OMEGA0_DOT, double omegaE, double t, double toe);
    extern double sv_geocentric_distance(double a, double e, double fs);
    extern double sv_orbit_inclination(double i0, double IDOT, double t);

    struct Perturb_correction
    {
        double du, dr, di;
    };
    extern Perturb_correction sv_perturb_corr(double latitude_argument, double Cuc, double Cus, double Crc, double Crs, double Cic, double Cis);
    extern std::tuple<double, double, double> sv_apply_perturb_correction(Perturb_correction const &corr, double latitude_argument, double geocentric_distance, double orbit_inclination);

    extern std::tuple<double, double, double> sv_orbit_coordinate(double latitude_argument, double orbit_inclination);
    extern std::tuple<double, double, double> sv_instance_earth_coordinate(double orbit_x, double orbit_y, double orbit_inclination, double longitude_argument);
}