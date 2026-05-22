#pragma once

#include <tuple>

#include <gaa/core/pp.hpp>
#include <gaa/wrap/Eigen/pre.hpp>
#include <gaa/wrap/boost/units.hpp>

namespace gaa {
using Collinearity_condition = Eigen::Matrix<double, 2, 6>;

inline decltype(auto) decompose_cc(Collinearity_condition const &cc) {
  return std::make_tuple(
      cc(0, 0), cc(0, 1), cc(0, 2), cc(0, 3), cc(0, 4), cc(0, 5),

      cc(1, 0), cc(1, 1), cc(1, 2), cc(1, 3), cc(1, 4), cc(1, 5));
}

#define GAA_decompose_cc(PREFFIX, COLLINEARITY_CONDITION)                      \
  auto [GAA_PP_REPEAT(6, PREFIX##_1), GAA_PP_REPEAT(6, PREFIX##_2)] =          \
      decompose_cc(COLLINEARITY_CONDITION)

struct Collinearity_condition_param {
  double x;
  double y;
  double f;
  double h;
  Radian k;
  Radian w;
  double z;
  Mat const &rot;
};

using Collinearity_condition_solver =
    Collinearity_condition (*)(Collinearity_condition_param);
extern Collinearity_condition cc_solve_all(Collinearity_condition_param p);
extern Collinearity_condition cc_solve_kappa(Collinearity_condition_param p);
extern Collinearity_condition cc_solve_none(Collinearity_condition_param p);
} // namespace gaa