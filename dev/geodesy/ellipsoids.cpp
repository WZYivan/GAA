#include <gaa/core/math.hpp>
#include <gaa/geodesy/ellipsoid.hpp>
#include <print>

int main() {
  gaa_assert(gaa::cgcs2000 != gaa::wgs84);
  for (int i = 1; i != 75 / 5 + 1; ++i) {
    double B(gaa::deg2rad(i * 5));
    double S = gaa::cgcs2000.meridian_arc_length(B);
    double Bf = gaa::cgcs2000.meridian_arc_bottom_latitude(S);
    std::println("B = {}\t S = {:.20f}\t Bf = {}\t dif = {}s", B, S, Bf,
                 gaa::rad2sec(std::abs(B - Bf)));
  }
}