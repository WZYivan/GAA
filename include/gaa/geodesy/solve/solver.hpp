#pragma once

#include <gaa/geodesy/ellipsoid.hpp>

namespace gaa {
struct Geodetic_solve_result {
  Latitude latitude;
  Longitude longitude;
  Azimuth rangle;
  Ellipsoid_ref ellipsoid;
};

struct Geodetic_rsolve_result {
  Azimuth angle;
  Azimuth rangle;
  double s;
  Ellipsoid_ref ellipsoid;
};

template <> struct Geodetic_coordinate_caster<Geodetic_solve_result> {
  static Geodetic_coordinate cast(Geodetic_solve_result const &v) {
    return Geodetic_coordinate(v.latitude, v.longitude, v.ellipsoid);
  }
  static Geodetic_coordinate cast_to(Geodetic_solve_result const &v,
                                     Ellipsoid const &e) {
    return Geodetic_coordinate(v.latitude, v.longitude, e);
  }
};
} // namespace gaa