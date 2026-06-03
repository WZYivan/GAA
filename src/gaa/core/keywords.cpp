#include <iostream>
#include <print>

#include <gaa/core/keywords.hpp>

namespace gaa {
void _assert_fail(char const *expr, std::source_location const &loc,
                  std::string const &message) {
  std::print(std::cerr,
             "Assertion failed\n"
             "file: {:s}:{:d}:{:d}\n"
             "function: {:s}\n"
             "expr: {:s}\n"
             "message: {:s}\n",
             loc.file_name(), loc.line(), loc.column(), loc.function_name(),
             expr, message);
}
#if GAA_LINK_WITH_CXX_EXP
void _assert_fail(char const *expr, std::source_location const &loc,
                  std::stacktrace const &stk, std::string const &message) {
  std::print(std::cerr,
             "Assertion failed\n"
             "file: {:s}:{:d}:{:d}\n"
             "function: {:s}\n"
             "expr: {:s}\n"
             "message: {:s}\n"
             "stacktrace:\n{}\n",
             loc.file_name(), loc.line(), loc.column(), loc.function_name(),
             expr, message, stk);
}
#endif
} // namespace gaa