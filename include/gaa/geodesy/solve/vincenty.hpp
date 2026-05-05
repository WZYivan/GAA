#pragma once

#include <gaa/core/kw.hpp>
#include <gaa/geodesy/solve/solver.hpp>

namespace gaa {
struct Vincenty_solver {
  Ellipsoid const &ellipsoid;

  ~Vincenty_solver() = default;
  Vincenty_solver(Ellipsoid const &e) : ellipsoid(e) {}

  Geodetic_solve_result solve(Latitude latitude, Longitude longitude, double s,
                              Azimuth angle, kwargs args = {}) const;
  Geodetic_rsolve_result rsolve(Latitude lat1, Longitude lon1, Latitude lat2,
                                Longitude lon2, kwargs args = {}) const;
};

struct _vincenty_solve_fn {
  double s;
  Azimuth angle;
  kwargs args;

  Geodetic_solve_result operator()(Geodetic_coordinate const &gc) const {
    return Vincenty_solver(gc.ellipsoid)
        .solve(gc.latitude, gc.longitude, s, angle, args);
  }
};

inline GAA_weak_channel((Geodetic_solve_result), (Geodetic_coordinate),
                        const &gc, (_vincenty_solve_fn), const &fn) {
  return fn(gc);
}

struct _vincenty_solve {
  _vincenty_solve_fn operator()(double s, Azimuth angle,
                                kwargs args = {}) const {
    return {s, angle, args};
  }
};

inline constexpr _vincenty_solve vincenty = {};

struct _vincenty_rsolve_fn {
  Latitude lat2;
  Longitude lon2;
  kwargs args;
  Ellipsoid const &ellipsoid;

  Geodetic_rsolve_result operator()(Geodetic_coordinate const &gc) const {
    gaa_assert(ellipsoid.is_null() || ellipsoid == gc.ellipsoid);
    return Vincenty_solver(gc.ellipsoid)
        .rsolve(gc.latitude, gc.longitude, lat2, lon2, args);
  }
};

inline GAA_weak_channel((Geodetic_rsolve_result), (Geodetic_coordinate),
                        const &gc, (_vincenty_rsolve_fn), const &fn) {
  return fn(gc);
}

struct _vincenty_rsolve {
  _vincenty_rsolve_fn operator()(Latitude lat2, Longitude lon2,
                                 kwargs args = {}) const {
    return _vincenty_rsolve_fn{lat2, lon2, args, null_ellipsoid};
  }

  _vincenty_rsolve_fn operator()(Geodetic_coordinate const &gc,
                                 kwargs args = {}) const {
    return _vincenty_rsolve_fn{gc.latitude, gc.longitude, args, gc.ellipsoid};
  }
};

inline constexpr _vincenty_rsolve rvincenty = {};

} // namespace gaa