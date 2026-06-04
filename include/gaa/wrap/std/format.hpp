/*
dispatch of <format>, some version of gcc may not specialize for bool or range
*/

#pragma once

#include <format>

template <> struct std::formatter<bool, char> {
  constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }
  auto format(bool b, std::format_context &ctx) const {
    return std::format_to(ctx.out(), "{}", b ? "true" : "false");
  }
};