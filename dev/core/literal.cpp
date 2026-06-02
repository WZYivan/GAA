#include <print>

#include <gaa/core/literal.hpp>

int main() {
  gaa_assert(gaa::literal_cast<int>("1") == 1);
  gaa_assert(gaa::literal_cast<double>("0.314e1") == 3.14);
  gaa_assert(gaa::literal_cast<double>("0.314D1") == 3.14);
  gaa_assert(gaa::literal_cast<gaa::units::Radian>("3.14_rad") ==
             gaa::rad(3.14));
}