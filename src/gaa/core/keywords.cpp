#include <iostream>
#include <print>

#include <gaa/core/keywords.hpp>

namespace gaa {
void _assert_fail(char const *expr, std::source_location const &loc,
                  std::string const &message) {
  std::print(std::cerr,
             "Assertion failed\n"
             "file: {:s}\n"
             "function: {:s}\n"
             "position: {:d}:{:d}\n"
             "expr: {:s}\n"
             "message: {:s}\n",
             loc.file_name(), loc.function_name(), loc.line(), loc.column(),
             expr, message);
}
} // namespace gaa