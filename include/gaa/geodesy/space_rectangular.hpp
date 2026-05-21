#pragma once

#include <gaa/core/kw.hpp>
#include <gaa/geodesy/ellipsoids.hpp>

namespace gaa {
struct Space_rectangular_coordinate {
  double x;
  double y;
  double z;

  Ellipsoid const &ellipsoid = null_ellipsoid;
};

template <> struct Geodetic_coordinate_caster<Space_rectangular_coordinate> {
  static Geodetic_coordinate cast(Space_rectangular_coordinate const &src,
                                  kwargs args = {}) {
    return cast_to(src, src.ellipsoid, args);
  }

  static Geodetic_coordinate cast_to(Space_rectangular_coordinate const &src,
                                     Ellipsoid const &e, kwargs args = {}) {
    GAA_ARG_OR(args, converge_threshold, 1e-10);

    auto const &geom = e.geometry();
    double x = src.x, y = src.y, z = src.z;
    double e2 = geom.e1_2();
    double a = geom.a();
    double bottom = std::sqrt(x * x + y * y);
    double B0 = std::atan(z / bottom), B = B0;

    double N;
    {
      double delta = 1e10;
      while (std::abs(delta) > converge_threshold) {
        B0 = B;
        double sinB = std::sin(B);
        double sin2B = std::pow(sinB, 2);
        N = a / std::sqrt(1 - e2 * sin2B);
        B = std::atan((z + N * e2 * sinB) / bottom);
        delta = B - B0;
      }
    }

    double L = std::atan(y / x);

    if (L < 0) {
      L += std::numbers::pi;
    } else if (L > std::numbers::pi) {
      L -= std::numbers::pi;
    }
    double H = z / std::sin(B) - N * (1 - e2);

    return Geodetic_coordinate(lat(B), lon(L), H, e);
  }
};
} // namespace gaa