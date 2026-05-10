#include <gaa/geodesy/solve/vincenty.hpp>
#include <numbers>

namespace gaa {
Geodetic_solve_result Vincenty_solver::solve(Latitude latitude,
                                             Longitude longitude, double s,
                                             Azimuth angle, kwargs args) const {
  GAA_ARG_OR(args, converge_threshold, 1e-12);

  double f = ellipsoid.geometry().alpha();
  double a = ellipsoid.geometry().a();
  double b = ellipsoid.geometry().b();
  double Phi1 = rad(latitude);
  double L1 = rad(longitude);
  double a1 = rad(angle);
  double cos_a1 = std::cos(a1);
  double sin_a1 = std::sin(a1);

  double U1 = std::atan((1 - f) * std::tan(Phi1));
  double sinU1 = std::sin(U1);
  double cosU1 = std::cos(U1);

  double sigma1 = std::atan2(std::tan(U1), cos_a1);
  double sin_alpha = std::cos(U1) * std::sin(a1);
  double sin_2_alpha = std::pow(sin_alpha, 2);
  double cos_2_alpha = 1 - sin_2_alpha;

  double u_2 = cos_2_alpha * ((std::pow(a, 2) / std::pow(b, 2)) - 1);
  double A =
      1 + u_2 / 16384.0 * (4096 + u_2 * (-768 + u_2 * (320 - 175 * u_2)));
  double B = u_2 / 1024.0 * (256 + u_2 * (-128 + u_2 * (74 - 47 * u_2)));

  double sigma;
  double cos_d_sigma_m;
  double cos_2_d_sigma_m;
  {
    double init = s / (b * A);
    sigma = init;
    double prev;

    do {
      double d_sigma_m = 2 * sigma1 + sigma;

      double sin_sigma = std::sin(sigma);
      double sin_2_sigma = std::pow(sin_sigma, 2);
      double cos_sigma = std::cos(sigma);
      double cos_2_sigma = std::pow(cos_sigma, 2);
      cos_d_sigma_m = std::cos(d_sigma_m);
      cos_2_d_sigma_m = std::pow(cos_d_sigma_m, 2);

      double delta_sigma =
          B * sin_sigma *
          (cos_d_sigma_m +
           1.0 / 4.0 * B *
               (cos_sigma * (-1 + 2 * cos_2_d_sigma_m) -
                1.0 / 6.0 * B * cos_d_sigma_m * (-3 + 4 * sin_2_sigma) *
                    (-3 + 4 * cos_2_sigma)));
      prev = sigma;
      sigma = init + delta_sigma;
    } while (!within_abs(sigma, prev, converge_threshold));
  }
  double sin_sigma = std::sin(sigma);
  double cos_sigma = std::cos(sigma);

  double Phi2 = std::atan2(
      sinU1 * cos_sigma + cosU1 * sin_sigma * cos_a1,
      (1 - f) * std::sqrt(sin_2_alpha + std::pow(sinU1 * sin_sigma -
                                                     cosU1 * cos_sigma * cos_a1,
                                                 2)));
  double lambda = std::atan2(sin_sigma * sin_a1,
                             cosU1 * cos_sigma - sinU1 * sin_sigma * cos_a1);
  double C = f / 16.0 * (4 + f * (4 + 3 * cos_2_alpha));
  double L =
      lambda - (1 - C) * f * sin_alpha *
                   (sigma + C * sin_sigma *
                                (cos_d_sigma_m +
                                 C * cos_sigma * (-1 + 2 * cos_2_d_sigma_m)));
  double L2 = L + L1;
  double a2 =
      std::atan2(sin_alpha, -sinU1 * sin_sigma + cosU1 * cos_sigma * cos_a1);
  a2 += std::numbers::pi;
  return Geodetic_solve_result{.latitude = Latitude(Phi2),
                               .longitude = Longitude(L2),
                               .rangle = Azimuth(a2),
                               .ellipsoid = ellipsoid};
}

Geodetic_rsolve_result Vincenty_solver::rsolve(Latitude lat1, Longitude lon1,
                                               Latitude lat2, Longitude lon2,
                                               kwargs args) const {
  GAA_ARG_OR(args, converge_threshold, 1e-12);

  double f = ellipsoid.geometry().alpha();
  double a = ellipsoid.geometry().a();
  double b = ellipsoid.geometry().b();

  double Phi1 = rad(lat1);
  double Phi2 = rad(lat2);
  double L1 = rad(lon1);
  double L2 = rad(lon2);

  double U1 = std::atan((1 - f) * std::tan(Phi1));
  double U2 = std::atan((1 - f) * std::tan(Phi2));
  double L = L2 - L1;
  double lambda = L;
  gaa_assert(std::abs(lambda) <= std::numbers::pi);

  double sinU1 = std::sin(U1);
  double cosU1 = std::cos(U1);
  double sinU2 = std::sin(U2);
  double cosU2 = std::cos(U2);
  double sin_lambda = std::sin(lambda);
  double cos_lambda = std::cos(lambda);

  double cos_2_alpha;
  double sin_sigma;
  double cos_sigma;
  double cos_2_d_sigma_m;
  double cos_d_sigma_m;
  double sigma;
  {
    double prev;
    do {
      sin_lambda = std::sin(lambda);
      cos_lambda = std::cos(lambda);

      sin_sigma =
          std::sqrt(std::pow(cosU2 * sin_lambda, 2) +
                    std::pow(cosU1 * sinU2 - sinU1 * cosU2 * cos_lambda, 2));
      cos_sigma = sinU1 * sinU2 + cosU1 * cosU2 * cos_lambda;
      sigma = std::atan2(sin_sigma, cos_sigma);
      double sin_alpha = (cosU1 * cosU2 * sin_lambda) / sin_sigma;
      cos_2_alpha = 1 - std::pow(sin_alpha, 2);
      cos_d_sigma_m = cos_sigma - (2 * sinU1 * sinU2) / (cos_2_alpha);
      cos_2_d_sigma_m = std::pow(cos_d_sigma_m, 2);
      double C = f / 16.0 * cos_2_alpha * (4.0 + f * (4.0 - 3.0 * cos_2_alpha));
      prev = lambda;
      lambda =
          L + (1 - C) * f * sin_alpha *
                  (sigma + C * sin_sigma *
                               (cos_d_sigma_m +
                                C * cos_sigma * (-1 + 2 * cos_2_d_sigma_m)));
    } while (!within_abs(lambda, prev, converge_threshold));
  }
  double sin_2_sigma = std::pow(sin_sigma, 2);

  double u_2 = cos_2_alpha * ((std::pow(a, 2) / std::pow(b, 2)) - 1);
  double A =
      1 + u_2 / 16384.0 * (4096 + u_2 * (-768 + u_2 * (320 - 175 * u_2)));
  double B = u_2 / 1024.0 * (256 + u_2 * (-128 + u_2 * (74 - 47 * u_2)));
  double delta_sigma =
      B * sin_sigma *
      (cos_d_sigma_m +
       1.0 / 4.0 * B *
           (cos_sigma * (-1 + 2 * cos_2_d_sigma_m) -
            1.0 / 6.0 * B * cos_d_sigma_m * (-3 + 4 * sin_2_sigma) *
                (-3 + 4 * cos_2_d_sigma_m)));
  double s = b * A * (sigma - delta_sigma);
  double a1 = std::atan2(cosU2 * sin_lambda,
                         cosU1 * sinU2 - sinU1 * cosU2 * cos_lambda);
  double a2 = std::atan2(cosU1 * sin_lambda,
                         -sinU1 * cosU2 + cosU1 * sinU2 * cos_lambda);
  a1 += std::numbers::pi;
  a2 += std::numbers::pi;
  a2 += (a1 < std::numbers::pi) ? std::numbers::pi : 0;
  return Geodetic_rsolve_result{.angle = Azimuth(a1),
                                .rangle = Azimuth(a2),
                                .s = s,
                                .ellipsoid = ellipsoid};
}
} // namespace gaa