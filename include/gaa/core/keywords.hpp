#pragma once

#include <cassert>
#include <cstdlib>
#include <source_location>
#include <string>

#include <gaa/core/config.hpp>

namespace gaa {
extern void _assert_fail(char const *expr, std::source_location const &loc,
                         std::string const &message = "");
}

#define gaa_here (std::source_location::current())

#ifndef GAA_USE_STD_ASSERT
#define gaa_assert(EXPR, ...)                                                  \
  (EXPR) ? (void(0))                                                           \
         : (gaa::_assert_fail(#EXPR, gaa_here __VA_OPT__(, ) __VA_ARGS__),     \
            GAA_AFTER_ASSERTION_FAIL)
#else
#define gaa_assert(EXPR, ...) assert(EXPR)
#endif

#ifndef GAA_ENABLE_MODULE
#define GAA_EXPORT
#define GAA_MODULE
#define GAA_EXPORT_MODULE(NAME)
#else
#define GAA_EXPORT export
#define GAA_MODULE module
#define GAA_EXPORT_MODULE(NAME) GAA_EXPORT GAA_MODULE NAME
#endif
