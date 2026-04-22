#pragma once

#include <cassert>
#include <cstdlib>
#include <source_location>
#include <string>

#include <gaa/core/config.hpp>

namespace gaa
{
    extern void _assert_fail(char const *expr, std::source_location const &loc, std::string const &message = "");
}

#define gaa_here (std::source_location::current())

#ifndef GAA_USE_STD_ASSERT
#define gaa_assert(EXPR, ...) (EXPR) ? (void(0)) : (gaa::_assert_fail(#EXPR, gaa_here __VA_OPT__(, ) __VA_ARGS__), GAA_exit_fail())
#else
#define gaa_assert(EXPR, ...) assert(EXPR)
#endif
