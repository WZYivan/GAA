#include <iostream>
#include <print>

// #define GAA_UNIT_NEVER_VALIDATE
#include <gaa/core/keywords.hpp>
#include <gaa/core/math.hpp>
#include <gaa/core/units.hpp>

void LB(gaa::Latitude, gaa::Longitude) {}

int main() {
  // gaa::Arcdeg deg(180);
  // gaa_assert(deg == 180 * gaa::Arcdeg::unit);

  // gaa::Radian rad(deg);
  // gaa_assert(rad.value() != deg.value());

  // gaa::Arcmin min(deg);
  // gaa_assert(gaa::within_abs(min.value(), deg.value() * 60));

  // gaa::Arcsec sec(0.001);
  // gaa::Latitude b1(deg / (4 * deg.unit)), b2(rad / (10 * rad.unit)),
  //     b3(sec / (100 * sec.unit)), b4(GAA_UNITS_OP(gaa::Radian(10), /, 10));

  // GAA_UNITS_OP(deg, /, 4);

  // LB(gaa::Latitude(0), gaa::Longitude(0));

  // gaa::Latitude _(10089);
  // std::cout << gaa::rad(_);

  // std::sin(gaa::rad(gaa::Latitude(0.1)));

  auto b = gaa::lat(-0.2);
  std::cout << gaa::rad(b);

  return 0;
}