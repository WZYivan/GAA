#pragma once

#include <source_location>
#include <string>

#include <gaa/core/buildsystem/config.hpp>
#include <gaa/core/pp.hpp>

#if GAA_LINK_WITH_CXX_EXP
#include <stacktrace>
#endif

namespace gaa {
/// for std::stacktrace
/// gcc => libstdc++exp
/// clang => libc++experimental
#if GAA_LINK_WITH_CXX_EXP
extern void _assert_fail(char const *expr, std::source_location const &loc,
                         std::stacktrace const &stk,
                         std::string const &message = "");
#endif
extern void _assert_fail(char const *expr, std::source_location const &loc,
                         std::string const &message = "");
} // namespace gaa

/// using std::stacktrace version
#if GAA_LINK_WITH_CXX_EXP
#define gaa_here std::source_location::current(), std::stacktrace::current()
#else
#define gaa_here std::source_location::current()
#endif

#ifndef GAA_USE_STD_ASSERT
/*
 * runtime assert support format message and source location infomation (auto)
 * and stacktrace (if possible) output
 * usage:
 * gaa_assert(1+1 == 3, "Are U {}?", "stupid ass")
 *
 * it output looks like:
 * Assertion failed
 * file: /home/azusa/file/project/GAA/include/gaa/wrap/std/any/any.hpp:171:11
 * function: gaa::any::Vtable::init<std::vector<int, std::allocator<int> >
 * >()::<lambda(gaa::any::Any&, const gaa::any::Any&)> expr: elem.type_info()
 * == typeid(typename D::value_type) message: push_back argument type mismatch:
 * given std::__cxx11::basic_string<char, std::char_traits<char>,
 * std::allocator<char> /// >, expect int stacktrace: > 0#
 * /home/azusa/file/project/GAA/include/gaa/wrap/std/any/any.hpp:171
 * [106604742805834]
 *         gaa::any::Vtable::init<std::vector<int, std::allocator<int> >
 *         >()::{lambda(gaa::any::Any&, gaa::any::Any const&)#1}///
 *         ::operator()(gaa::any::Any&, gaa::any::Any const&) const
 * $ 1# <STL> /usr/include/c++/15/bits/invoke.h:63 [106604742866516]
 * $ 2# <STL> /usr/include/c++/15/bits/invoke.h:113 [106604742851953]
 * $ 3# <STL> /usr/include/c++/15/bits/std_function.h:292 [106604742836723]
 * $ 4# <STL> /usr/include/c++/15/bits/std_function.h:593 [106604742786201]
 * > 5# /home/azusa/file/project/GAA/include/gaa/wrap/std/any/any.hpp:108
 * [106604742768420]
 *         void gaa::any::Any::push_back<char const (&) [19]>(char const (&)
 *         [19])
 * > 6# /home/azusa/file/project/GAA/dev/wrap/std/any.cpp:13 [106604742745392]
 *         main
 * * 7# <compiler> __libc_start_call_main =>
 * ../sysdeps/nptl/libc_start_call_main.h:58 [124713640829385]
 * * 8# <compiler> __libc_start_main_impl => ../csu/libc-start.c:360
 * [124713640829578]
 * * 9# <compiler> _start => :0 [106604742744708]
 * * 10# <compiler>  => :0 [18446744073709551615]
 * [1] 64977 IOT instruction(core dumped) ./dev/any
 */
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
/// or you can go back to standard assert
#include <cassert>
#define gaa_assert(EXPR, ...) assert(EXPR)
#endif

/// incomplete part
#ifndef GAA_ENABLE_MODULE
#define GAA_EXPORT
#define GAA_MODULE
#define GAA_EXPORT_MODULE(NAME)
#else
#define GAA_EXPORT export
#define GAA_MODULE module
#define GAA_EXPORT_MODULE(NAME) GAA_EXPORT GAA_MODULE NAME
#endif
/// incomplete part

namespace gaa {
/* ouput unmangled typename etc., using <cxxabi.h>
 * example:
 * raw: NSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE
 * demangle : std::__cxx11::basic_string<char, std::char_traits<char>,
 *  std::allocator<char> >
 */
extern std::string demangle(const char *name);
} // namespace gaa
