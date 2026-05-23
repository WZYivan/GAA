#pragma once

#include <array>

#include <gaa/wrap/std/vectorize/expr.hpp>

namespace gaa::vectorize {
template <class V, auto S> struct Register_expr<std::array<V, S>> {
  GAA_VECTORIZE_ALLOW();
};
} // namespace gaa::vectorize