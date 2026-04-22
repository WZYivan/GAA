#include <cmath>

#include <gaa/gnss/space/sv_orbit.hpp>

namespace gaa
{
    double sv_mean_angular_velocity(double GM, double sqrtA)
    {
        return std::sqrt(GM) / std::pow(sqrtA, 3);
    }

    double sv_eccentric_anomaly(double mean_anomaly, double e, kwargs args)
    {
        double epsilon = args._has_converge_threshold() ? args.converge_threshold() : 1e-10;
        double Es0 = mean_anomaly;
        double delta = epsilon * 2;

        while (!(std::abs(delta) < epsilon))
        {
            double Es1 = mean_anomaly + e * std::sin(Es0);
            delta = Es1 - Es0;
            Es0 = Es1;
        }

        return Es0;
    }

    double sv_true_anomaly(double eccentric_anomaly, double e)
    {
        return 2 * std::atan(std::tan(eccentric_anomaly / 2) * pow((1 + e) / (1 - e), 0.5));
    }

    double sv_geocentric_distance(double a, double e, double fs)
    {
        return (a * (1 - std::pow(e, 2))) / (1 + e * std::cos(fs));
    }

    double sv_latitude_argument(double true_anomaly, double omega)
    {
        return true_anomaly + omega;
    }

    double sv_longitude_argument(double OMEGA0, double OMEGA0_DOT, double omegaE, double t, double toe)
    {
        return OMEGA0 + (OMEGA0_DOT - omegaE) * t - OMEGA0_DOT * toe;
    }

    double sv_orbit_inclination(double i0, double IDOT, double t)
    {
        return i0 + IDOT * t;
    }

    Perturb_correction sv_perturb_corr(double latitude_argument, double Cuc, double Cus, double Crc, double Crs, double Cic, double Cis)
    {
        Perturb_correction corr;
        double
            cos = std::cos(2 * latitude_argument),
            sin = std::sin(2 * latitude_argument);
        corr.du = Cuc * cos + Cus * sin;
        corr.dr = Crc * cos + Crs * sin;
        corr.di = Cic * cos + Cis * sin;

        return corr;
    }

    std::tuple<double, double, double> sv_apply_perturb_correction(Perturb_correction const &corr, double latitude_argument, double geocentric_distance, double orbit_inclination)
    {
        double u = latitude_argument + corr.du;
        double r = geocentric_distance + corr.dr;
        double i = orbit_inclination + corr.di;
        return std::make_tuple(u, r, i);
    }

    std::tuple<double, double, double> sv_orbit_coordinate(double latitude_argument, double geodetic_distance)
    {
        return std::make_tuple(
            geodetic_distance * std::cos(latitude_argument),
            geodetic_distance * std::sin(latitude_argument),
            0.0);
    }

    std::tuple<double, double, double> sv_instance_earth_coordinate(double orbit_x, double orbit_y, double orbit_inclination, double longitude_argument)
    {
        double
            cosL = std::cos(longitude_argument),
            sinL = std::sin(longitude_argument),
            cosI = std::cos(orbit_inclination),
            sinI = std::sin(orbit_inclination);
        return std::make_tuple(
            orbit_x * cosL - orbit_y * cosI * sinL,
            orbit_x * sinL + orbit_y * cosI * cosL,
            orbit_y * sinI);
    }
}