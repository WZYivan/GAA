#define GAA_AFTER_ASSERTION_FAIL void(0)

#include <cxxabi.h>
#include <gaa/core/keywords.hpp>
#include <iostream>
#include <memory>
#include <print>

int main() {
  gaa_assert(false, "{}", false);
  char const *cstr = typeid(std::string).name();
  std::println("{} => {}", cstr, gaa::demangle(cstr));
}