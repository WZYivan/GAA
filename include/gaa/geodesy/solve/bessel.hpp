#pragma once

#include <gaa/core/kw.hpp>
#include <gaa/core/signature_of_deprecated_header.hpp>
#include <gaa/geodesy/solve/solver.hpp>
// #include <print>

namespace gaa {
struct Bessel_solver {
  Ellipsoid const &ellipsoid;

  Geodetic_solve_result solve(Latitude latitude, Longitude longitude, double s,
                              Azimuth angle) const;
  Geodetic_rsolve_result rsolve(Latitude lat1, Longitude lon1, Latitude lat2,
                                Longitude lon2, kwargs args = {}) const;

  ~Bessel_solver() = default;
  Bessel_solver(Ellipsoid const &e) : ellipsoid(e) {}
};

struct _bessel_solve_fn_gc {
  double s;
  Azimuth angle;

  Geodetic_solve_result operator()(Geodetic_coordinate const &gc) const {
    return Bessel_solver(gc.ellipsoid)
        .solve(gc.latitude, gc.longitude, s, angle);
  }
};

struct _bessel_solve_fn_grr {
  Latitude lat1;
  Longitude lon1;

  Geodetic_solve_result operator()(Geodetic_rsolve_result const &r) const {
    return Bessel_solver(r.ellipsoid).solve(lat1, lon1, r.s, r.angle);
  }
};

inline GAA_weak_channel((Geodetic_solve_result), (Geodetic_coordinate),
                        const &gc, (_bessel_solve_fn_gc), const &fn) {
  return fn(gc);
}

inline GAA_weak_channel((Geodetic_solve_result), (Geodetic_rsolve_result),
                        const &r, (_bessel_solve_fn_grr), const &fn) {
  return fn(r);
}

struct _bessel_solve {
  _bessel_solve_fn_gc operator()(double s, Azimuth angle) const {
    return {s, angle};
  }

  _bessel_solve_fn_grr operator()(Latitude lat1, Longitude lon1) const {
    return {lat1, lon1};
  }
};

inline constexpr _bessel_solve bessel = {};

struct _bessel_rsolve_fn {
  Latitude lat1_2;
  Longitude lon1_2;
  kwargs args;

  Geodetic_rsolve_result operator()(Geodetic_solve_result const &r) const {
    // std::println("(rbessel.gr) L1 = {}, B1 = {}, L2 = {}, B1 = {}",
    // rad(lat1_2),
    //              rad(lon1_2), rad(r.latitude), rad(r.longitude));
    return Bessel_solver(r.ellipsoid)
        .rsolve(lat1_2, lon1_2, r.latitude, r.longitude, args);
  }

  Geodetic_rsolve_result operator()(Geodetic_coordinate const &gc) const {
    // std::println("(rbessel.gc) L1 = {}, B1 = {}, L2 = {}, B1 = {}",
    //              rad(gc.latitude), rad(gc.longitude), rad(lat1_2),
    //              rad(lon1_2));
    return Bessel_solver(gc.ellipsoid)
        .rsolve(gc.latitude, gc.longitude, lat1_2, lon1_2, args);
  }
};

inline GAA_weak_channel((Geodetic_rsolve_result), (Geodetic_solve_result),
                        const &r, (_bessel_rsolve_fn), const &fn) {
  return fn(r);
}

inline GAA_weak_channel((Geodetic_rsolve_result), (Geodetic_coordinate),
                        const &gc, (_bessel_rsolve_fn), const &fn) {
  return fn(gc);
}

struct _bessel_rsolve {
  _bessel_rsolve_fn operator()(Latitude lat1_2, Longitude lon1_2,
                               kwargs args = {}) const {
    return _bessel_rsolve_fn{lat1_2, lon1_2, args};
  }
};

inline constexpr _bessel_rsolve rbessel = {};
} // namespace gaa