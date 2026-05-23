#include <iostream>
#include <print>
#include <ranges>
#include <vector>

#include <Eigen/Dense>

#define GAA_VECTORIZE_REGISTER_VIEW
#define GAA_VECTORIZE_REGISTER_VECTOR
#define GAA_VECTORIZE_REGISTER_ARRAY
#define GAA_VECTORIZE_REGISTER_EIGEN_DENSE
#define GAA_VECTORIZE_REGISTER_ARITHMETIC
#include <gaa/wrap/std/vectorize/operators.hpp>

#include <nameof.hpp>

int main() {
  using namespace gaa::vectorize::operators;
  std::vector<double> v1{1, 2, 3};
  std::array<int, 3> v2{4, 5, 6};
  std::vector<float> v3{7, 8, 9};
  std::vector<long> v4{10, 11, 12};
  Eigen::Vector3d v5{13, 14, 15};

  auto expr = -((v1 | std::views::transform([](auto x) { return x + 1; })) +
                v2 / v3 - v4 * v5 + 100 * std::views::iota(0, 3));
  auto vr = expr.eval();

  std::cout << NAMEOF_TYPE(decltype(expr)) << std::endl;

  for (auto v : vr) {
    std::println("{}", v);
  }
}