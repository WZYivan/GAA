#include <cxxabi.h>
#include <format>
#include <iostream>
#include <memory>
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

std::string demangle(const char *name) {
  int status = 0;
  std::unique_ptr<char, void (*)(void *)> result(
      abi::__cxa_demangle(name, nullptr, nullptr, &status), std::free);
  return (status == 0) ? result.get() : name;
}

#if GAA_LINK_WITH_CXX_EXP
void _print_stacktrace_for_assert_fail(std::stacktrace const &stk) {
  for (auto i = 0uz; i != stk.size(); ++i) {
    auto const &entry = stk.at(i);

    if (entry.source_file().contains("c++")) {
      std::print("$ {}# <STL> {}:{} [{}]\n", i, entry.source_file(),
                 entry.source_line(), entry.native_handle());
      continue;
    } else if (entry.description().starts_with("__") ||
               entry.description() == "_start" || entry.description().empty()) {
      std::print("* {}# <compiler> {} => {}:{} [{}]\n", i, entry.description(),
                 entry.source_file(), entry.source_line(),
                 entry.native_handle());
      continue;
    }

    std::print("> {}# {}:{} [{}]\n"
               "\t{}\n",
               i, entry.source_file(), entry.source_line(),
               entry.native_handle(), demangle(entry.description().c_str()));
  }
}
void _assert_fail(char const *expr, std::source_location const &loc,
                  std::stacktrace const &stk, std::string const &message) {
  std::print(std::cerr,
             "Assertion failed\n"
             "file: {:s}:{:d}:{:d}\n"
             "function: {:s}\n"
             "expr: {:s}\n"
             "message: {:s}\n"
             "stacktrace:\n",
             loc.file_name(), loc.line(), loc.column(), loc.function_name(),
             expr, message);
  _print_stacktrace_for_assert_fail(stk);
}
#endif
} // namespace gaa