#pragma once

#include <Eigen/Dense>

#include <gaa/core/buildsystem/config.hpp>
#include <gaa/wrap/Eigen/pp.hpp>

namespace gaa {
GAA_EIGEN_XPR_FN((bool), is_rows_like, 1, int rows) {
  return m_1.rows() == rows;
}

GAA_EIGEN_XPR_FN((bool), is_cols_like, 1, int cols) {
  return m_1.cols() == cols;
}

GAA_EIGEN_XPR_FN((bool), is_shape_like, 2) {
  return m_1.rows() == m_2.rows() && m_1.cols() == m_2.cols();
}

GAA_EIGEN_XPR_FN((bool), is_coordinates3, 1) {
  return is_cols_like(m_1, 3) && m_1.rows() != 0;
}

GAA_EIGEN_XPR_FN((bool), is_coordinates2, 1) {
  return is_cols_like(m_1, 2) && m_1.rows() != 0;
}

GAA_EIGEN_XPR_FN((bool), is_rotation_matrix, 1) {
  return is_rows_like(m_1, 3) && is_cols_like(m_1, 3);
}

GAA_EIGEN_XPR_FN((bool), is_square_matrix, 1) {
  return m_1.rows() == m_1.cols();
}

using Mat = Eigen::MatrixXd;
} // namespace gaa