#include <gaa/core/math.hpp>
#include <gaa/geodesy/project/gauss_kruger.hpp>
#include <print>

int main() {
  auto const &ellipsoid = gaa::cgcs2000;
  auto const &project = gaa::gauss_kruger.project(6.0);
  double L = gaa::deg2rad(115);
  for (int i = 1; i != 90; ++i) {
    double B = gaa::deg2rad(i);
    auto gc = ellipsoid.coordinate(B, L);
    auto igc = gc >> project >> ellipsoid;

    double rb = gaa::rad2sec((gc.latitude - igc.latitude).value()),
           rl = gaa::rad2sec((gc.longitude - igc.longitude).value());
    gaa_assert(gaa::within_abs(rb, 0, 1e-3));
    gaa_assert(gaa::within_abs(rl, 0, 1e-3));
  }
  double B = gaa::deg2rad(45);
  auto gc = ellipsoid.coordinate(B, L) >> project >> gaa::geodetic_cast >>
            project(23) >> project(20) >> gaa::geodetic_cast(ellipsoid);
  gaa_assert(gaa::within_abs(B, gc.latitude.value(), 1e-3));
  gaa_assert(gaa::within_abs(L, gc.longitude.value(), 1e-3));
}