#include <print>

#include <gaa/wrap/std/variant/overload.hpp>

int main() {
  using gaa::variant::overload;
  std::string prefix = "---";
  auto f = overload([](int const &x) { std::println("int = {}", x); },
                    [](double const &x) { std::println("double = {}", x); },
                    [&prefix](std::string const &str) {
                      std::println("string = {}{}", prefix, str);
                    });

  {
    int x = 2;
    f(x);
  }

  {
    double x = 3.14;
    f(x);
  }

  {
    std::string x = "str";
    f(x);
  }
}