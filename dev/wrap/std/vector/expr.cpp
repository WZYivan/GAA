#include <nameof.hpp>
#include <print>

#include <gaa/wrap/std/vector/expr.hpp>

int main() {
  using namespace gaa;
  Vecd v1{1, 2, 3};
  Vecd v2{4, 5, 6};
  Veci v3{7, 8, 9};
  Vecd vr = (-((v1 + v2 - v3) * v1 / v2 - v2) / 2);

  for (auto i = 0uz; i != vr.size(); ++i) {
    std::println("vr[{}] = {}", i, vr[i]);
  }

  std::println("{}",
               NAMEOF_TYPE(decltype(-((v1 + v2 - v3) * v1 / v2 - v2) / 2)));
}