#include <print>

#include <gaa/geodesy/space_rectangular.hpp>

int main() {
  using namespace gaa;
  Space_rectangular_coordinate src{
      .x = -237'591'8.915, .y = 4'734'801.711, .z = 3'540'220.314};
  auto geo = src >> geodetic_cast(cgcs2000);
  std::println("L = {}, B = {}, H = {}", rad2deg(rad(geo.longitude)),
               rad2deg(rad(geo.latitude)), geo.height);
}