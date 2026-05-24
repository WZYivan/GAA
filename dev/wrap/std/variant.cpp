#include <print>

#include <gaa/wrap/std/variant.hpp>

int main() {
  using namespace gaa;
  Variant var;
  var = 1;

  var = 11.2;

  int x;
  std::visit(visitor::assign(x), var);

  std::println("{}", x);
}