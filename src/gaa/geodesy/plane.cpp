#include <cmath>

#include <gaa/geodesy/plane.hpp>

namespace gaa {

std::tuple<double, Azimuth> xy2da(double x, double y) {
  double d = std::sqrt(std::pow(x, 2) + std::pow(y, 2));
  double a = std::atan(y / x);
  return std::make_tuple(d, Azimuth(a));
}

std::tuple<double, double> da2xy(double d, Azimuth a) {
  double r = rad(a);
  double x = std::sin(r) * d;
  double y = std::cos(r) * d;
  return std::make_tuple(x, y);
}

Plane_coordinate solve(Plane_coordinate pc, double d, Azimuth a) {
  double x = pc.x + d * std::cos(rad(a));
  double y = pc.y + d * std::sin(rad(a));
  return {x, y};
}

std::tuple<double, Azimuth> rsolve(Plane_coordinate pc1, Plane_coordinate pc2) {
  double dx = pc2.x - pc1.x;
  double dy = pc2.y - pc1.y;
  double d = std::sqrt(dx * dx + dy * dy);
  double a = std::atan2(dy, dx);
  return std::make_tuple(d, Azimuth(a));
}

} // namespace gaa