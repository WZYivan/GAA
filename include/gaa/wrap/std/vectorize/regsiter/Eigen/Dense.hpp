#pragma once

#include <Eigen/Core>

#include <gaa/wrap/std/vectorize/expr.hpp>

namespace gaa::vectorize {
template <class V, auto R, auto C>
struct Register_expr<Eigen::Matrix<V, R, C>> {
  GAA_VECTORIZE_ALLOW();
};
} // namespace gaa::vectorize