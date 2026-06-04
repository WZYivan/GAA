/*
preprocessor for generate Eigen expr functions
*/

#pragma once

#include <gaa/core/pp.hpp>

#define GAA_EIGEN_PP_XPR(N) GAA_PP_REPEAT(N, class Xpr)
#define GAA_EIGEN_PP_M_XPR_CREF(N)                                             \
  GAA_PP_REPEAT_SUFFIX(N, Eigen::MatrixBase < Xpr, (> const &))
#define GAA_EIGEN_PP_M_XPR_REF(N)                                              \
  GAA_PP_REPEAT_SUFFIX(N, Eigen::MatrixBase < Xpr, (> &))

#define GAA_EIGEN_PP_M_CREF(N)                                                 \
  GAA_PP_REPEAT2(N, Eigen::MatrixBase<Xpr, > const &m)
#define GAA_EIGEN_PP_M_REF(N) GAA_PP_REPEAT2(N, Eigen::MatrixBase<Xpr, > &m)

#define GAA_EIGEN_XPR_FN(RET, NAME, N, ...)                                    \
  template <GAA_EIGEN_PP_XPR(N)>                                               \
  GAA_PP_STRIP_PARAM RET NAME(GAA_EIGEN_PP_M_CREF(N) __VA_OPT__(, )            \
                                  __VA_ARGS__)

#define GAA_EIGEN_MUT_XPR_FN(RET, NAME, N, ...)                                \
  template <GAA_EIGEN_PP_XPR(N)>                                               \
  GAA_PP_STRIP_PARAM RET NAME(GAA_EIGEN_PP_M_REF(N) __VA_OPT__(, )             \
                                  __VA_ARGS__)
