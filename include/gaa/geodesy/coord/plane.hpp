#pragma once

#include <tuple>

#include <gaa/wrap/boost/units.hpp>

namespace gaa {
struct Plane_coordinate {
  double x;
  double y;
};

struct Plane_polar_coordinate {
  double distance;
  Azimuth azimuth;
};

extern std::tuple<double, Azimuth> xy2da(double x, double y);
extern std::tuple<double, double> da2xy(double d, Azimuth a);
extern Plane_coordinate solve(Plane_coordinate pc, double d, Azimuth a);
extern std::tuple<double, Azimuth> rsolve(Plane_coordinate pc1,
                                          Plane_coordinate pc2);
} // namespace gaa