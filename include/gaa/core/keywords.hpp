#pragma once

#include <format>
#include <source_location>
#include <string>

#include <gaa/core/buildsystem/config.hpp>

namespace gaa {
extern void _assert_fail(char const *expr, std::source_location const &loc,
                         std::string const &message = "");
}

#define gaa_here (std::source_location::current())

#ifndef GAA_USE_STD_ASSERT
/// runtime assert support format message and source location infomation (auto)
/// output
/// usage:
/// gaa_assert(1+1 == 3, "Are U {}?", "stupid ass")
#define gaa_assert(EXPR, ...)                                                  \
  (EXPR) ? (void(0))                                                           \
         : (gaa::_assert_fail(                                                 \
                #EXPR, gaa_here __VA_OPT__(, std::format(__VA_ARGS__))),       \
            GAA_AFTER_ASSERTION_FAIL)
#define gaa_fail(...)                                                          \
  gaa::_assert_fail("force to fail",                                           \
                    gaa_here __VA_OPT__(, std::format(__VA_ARGS__))),          \
      GAA_AFTER_ASSERTION_FAIL
#else
#include <cassert>
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
