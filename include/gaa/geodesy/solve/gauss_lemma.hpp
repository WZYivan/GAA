#pragma once

#include <gaa/core/kw.hpp>
#include <gaa/core/signature_of_deprecated_header.hpp>
#include <gaa/geodesy/solve/solver.hpp>

namespace gaa {
struct Gauss_lemma_solver {
  Ellipsoid const &ellipsoid;

  Geodetic_solve_result solve(Latitude latitude, Longitude longitude, double s,
                              Azimuth angle, kwargs args = {}) const;
  Geodetic_rsolve_result rsolve(Latitude lat1, Longitude lon1, Latitude lat2,
                                Longitude lon2) const;

  ~Gauss_lemma_solver() = default;
  Gauss_lemma_solver(Ellipsoid const &e) : ellipsoid(e) {}
};

struct _gauss_lemma_solve_fn_gc {

  double s;
  Azimuth angle;
  kwargs args;

  Geodetic_solve_result operator()(Geodetic_coordinate const &gc) const {
    return Gauss_lemma_solver(gc.ellipsoid)
        .solve(gc.latitude, gc.longitude, s, angle, args);
  }
};

struct _gauss_lemma_solve_fn_grr {
  Latitude lat1;
  Longitude lon1;
  kwargs args;

  Geodetic_solve_result operator()(Geodetic_rsolve_result const &r) const {
    return Gauss_lemma_solver(r.ellipsoid)
        .solve(lat1, lon1, r.s, r.angle, args);
  }
};

inline GAA_weak_channel((Geodetic_solve_result), (Geodetic_coordinate),
                        const &gc, (_gauss_lemma_solve_fn_gc), const &fn) {
  return fn(gc);
}

inline GAA_weak_channel((Geodetic_solve_result), (Geodetic_rsolve_result),
                        const &r, (_gauss_lemma_solve_fn_grr), const &fn) {
  return fn(r);
}

struct _gauss_lemma_solve {
  _gauss_lemma_solve_fn_grr operator()(Latitude lat1, Longitude lon1,
                                       kwargs args = {}) const {
    return {lat1, lon1, args};
  }

  _gauss_lemma_solve_fn_gc operator()(double s, Azimuth angle,
                                      kwargs args = {}) const {
    return {s, angle, args};
  }
};

inline constexpr _gauss_lemma_solve gauss_lemma = {};

struct _gauss_lemma_rsolve_fn {
  Latitude lat1_2;
  Longitude lon1_2;

  Geodetic_rsolve_result operator()(Geodetic_solve_result const &r) const {
    return Gauss_lemma_solver(r.ellipsoid)
        .rsolve(lat1_2, lon1_2, r.latitude, r.longitude);
  }

  Geodetic_rsolve_result operator()(Geodetic_coordinate const &gc) const {
    return Gauss_lemma_solver(gc.ellipsoid)
        .rsolve(gc.latitude, gc.longitude, lat1_2, lon1_2);
  }
};

inline GAA_weak_channel((Geodetic_rsolve_result), (Geodetic_solve_result),
                        const &r, (_gauss_lemma_rsolve_fn), const &fn) {
  return fn(r);
}

inline GAA_weak_channel((Geodetic_rsolve_result), (Geodetic_coordinate),
                        const &gc, (_gauss_lemma_rsolve_fn), const &fn) {
  return fn(gc);
}

struct _gauss_lemma_rsolve {
  _gauss_lemma_rsolve_fn operator()(Latitude lat1_2, Longitude lon1_2) const {
    return _gauss_lemma_rsolve_fn{lat1_2, lon1_2};
  }
};

inline constexpr _gauss_lemma_rsolve rgauss_lemma = {};
} // namespace gaa