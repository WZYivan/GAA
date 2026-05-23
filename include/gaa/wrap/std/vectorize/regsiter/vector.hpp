#pragma once

#include <vector>

#include <gaa/wrap/std/vectorize/expr.hpp>

namespace gaa::vectorize {
template <class V, class Alloc> struct Register_expr<std::vector<V, Alloc>> {
  GAA_VECTORIZE_ALLOW();
};
} // namespace gaa::vectorize