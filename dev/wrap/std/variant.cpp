#include <print>

#include <gaa/wrap/std/variant.hpp>

int main() {
  using namespace gaa;
  Variant var = {1, 2, 3};

  auto visit =
      gaa::make_visitor::Vec_Integer([](auto const &ints) {
        for (auto const &v : ints) {
          std::println("v = {}", v);
        }
      })
          .String([](auto const &str) {
            std::println("size of \'{}\' = {}", str, str.size());
          })
          .Double([](auto const &dbl) { gaa_assert(dbl == 1.1); })
          .Default([](auto const &var) { std::println("This is default"); });

  visit(var);
}