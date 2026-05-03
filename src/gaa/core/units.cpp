#include <gaa/core/units.hpp>

namespace gaa {
double rad(Radian const &r) { return r.value(); }
double rad(Latitude const &lat) { return lat.value(); }
double rad(Longitude const &lon) { return lon.value(); }
Radian rad(double r) { return Radian(r); }
} // namespace gaa