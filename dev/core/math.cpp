#include <gaa/core/math.hpp>
#include <print>
#include <ranges>

int main() {
  for (int i = 0; i != 10; ++i) {
    auto ptr = gaa::pascal_triangle_row(i);
    auto diff =
        gaa::difference(ptr | std::views::transform([](auto v) -> double {
                          return static_cast<double>(v);
                        }) | std::ranges::to<std::vector<double>>(),
                        1);
    for (auto v : ptr) {
      std::print("{} ", v);
    }
    std::print("=>");
    for (auto v : diff) {
      std::print("{} ", v);
    }
    std::println("");
  }
}