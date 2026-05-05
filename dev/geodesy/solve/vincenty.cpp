#include <gaa/geodesy/solve/bessel.hpp>
#include <gaa/geodesy/solve/vincenty.hpp>
#include <print>

int main() {
  gaa::Latitude B1(gaa::dms(47, 46, 52.647'0));
  gaa::Longitude L1(gaa::dms(35, 49, 36.330'0));
  gaa::Radian A1(gaa::dms(44, 12, 13.664));
  double S = 44'797.282'6;
  auto coord = gaa::cgcs2000.coordinate(B1, L1);

  auto bessel = coord >> gaa::bessel(S, A1);
  auto vincenty = coord >> gaa::vincenty(S, A1);

  std::println("bessel: L = {}, B = {}, A = {}", bessel.longitude.value(),
               bessel.latitude.value(), bessel.rangle.value());
  std::println("vincenty: L = {}, B = {}, A = {}", vincenty.longitude.value(),
               vincenty.latitude.value(), vincenty.rangle.value());

  auto rbessel = bessel >> gaa::rbessel(coord.latitude, coord.longitude);
  // auto rvincenty = vincenty >> gaa::geodetic_cast >> gaa::rvincenty(coord);
  auto rvincenty = coord >> gaa::rvincenty(vincenty >> gaa::geodetic_cast);
  std::println("A1 = {}, S = {}", A1.value(), S);
  std::println("rbessel: A1 = {}, A2 = {}, S = {}", rbessel.angle.value(),
               rbessel.rangle.value(), rbessel.s);
  std::println("rvincenty: A1 = {}, A2 = {}, S = {}", rvincenty.angle.value(),
               rvincenty.rangle.value(), rvincenty.s);
}