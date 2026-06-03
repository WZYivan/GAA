#include <gaa/wrap/std/any.hpp>
#include <print>

int main() {
  gaa::Any vec = {1, 2, 3};
  std::println("vec = {}", vec.format());

  vec.push_back(4);
  vec.push_back_literal("5.5");
  vec.push_back_literal("6.6", gaa::Literal_Type::Integer);
  std::println("vec = {} [size = {}]", vec.format(), vec.size());

  vec.push_back("mismatched element");
}