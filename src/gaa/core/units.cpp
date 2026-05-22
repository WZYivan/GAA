#include <gaa/core/units.hpp>

namespace gaa {
double rad(Radian const &r) { return r.value(); }
double rad(Latitude const &lat) { return lat.value(); }
double rad(Longitude const &lon) { return lon.value(); }

double deg(Radian const &r) { return rad2deg(rad(r)); }
double deg(Latitude const &lat) { return rad2deg(rad(lat)); }
double deg(Longitude const &lon) { return rad2deg(rad(lon)); }

Radian rad(double r) { return Radian(r); }
Latitude lat(double r) { return Latitude(r); }
Longitude lon(double r) { return Longitude(r); }
} // namespace gaa