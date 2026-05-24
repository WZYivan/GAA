#pragma once

#include <variant>

#include <gaa/wrap/c++/type.hpp>

namespace gaa {
using Variant = std::variant<Char, Integer, Numeric, String, Chars, Integers,
                             Numerics, Strings>;
using Variants = std::variant<Chars, Integers, Numerics, Strings>;

template <class... F> struct Variant_visitor : public F... {
  using F::operator()...;
};
template <class... F> Variant_visitor(F...) -> Variant_visitor<F...>;
template <class... F> constexpr auto make_visitor(F &&...f) {
  return Variant_visitor<F...>(f...);
}

namespace visitor {
inline constexpr auto null = make_visitor([](auto const &v) -> void {});

inline auto assign = [](auto &val) {
  return make_visitor([&val](auto const &x) -> void {
    if constexpr (requires { val = x; }) {
      val = x;
    } else {
      return;
    }
  });
};
} // namespace visitor
} // namespace gaa