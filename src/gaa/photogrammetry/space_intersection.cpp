#include "gaa/core/Eigen/utils.hpp"
#include "gaa/core/info.hpp"
#include "gaa/core/kw.hpp"
#include "gaa/photogrammetry/collinearity.hpp"
#include "gaa/photogrammetry/orient.hpp"
#include <gaa/photogrammetry/space_intersection.hpp>

namespace gaa {

Mat space_intersection(Orientated_image const &img1,
                       Orientated_image const &img2) {
  gaa_assert(is_coordinates2(img1.coord));
  gaa_assert(is_coordinates2(img2.coord));

  Mat isp_left = img1.coord, isp_right = img2.coord;

  isp_left.conservativeResize(Eigen::NoChange, 3);
  isp_left.col(2).fill(-img1.io.f);
  isp_right.conservativeResize(Eigen::NoChange, 3);
  isp_right.col(2).fill(-img2.io.f);

  Mat aux_left = isp2aux(isp_left, eo2rotation(img1.eo)),
      aux_right = isp2aux(isp_right, eo2rotation(img2.eo));
  double Xs1 = img1.eo.x, Xs2 = img2.eo.x, Ys1 = img1.eo.y, Ys2 = img2.eo.y,
         Zs1 = img1.eo.z, Zs2 = img2.eo.z;
  double BX = Xs2 - Xs1, BY = Ys2 - Ys1, BZ = Zs2 - Zs1;
  auto X1 = aux_left.col(0).array(), Y1 = aux_left.col(1).array(),
       Z1 = aux_left.col(2).array(), X2 = aux_right.col(0).array(),
       Y2 = aux_right.col(1).array(), Z2 = aux_right.col(2).array();
  auto N1 = (BX * Z2 - BZ * X2) / (X1 * Z2 - Z1 * X2),
       N2 = (BX * Z1 - BZ * X1) / (X1 * Z2 - Z1 * X2);
  auto X = Xs1 + BX + N2 * X2;
  auto Y = Ys1 + (N1 * Y1 + N2 * Y2 + BY) / 2.0;
  auto Z = Zs1 + BZ + N2 * Z2;

  Mat result(X.rows(), 3);
  result.col(0) << X.matrix();
  result.col(1) << Y.matrix();
  result.col(2) << Z.matrix();
  return result;
}

Space_intersection_ols_result
space_intersection_ols(std::span<Orientated_image const> list, kwargs args) {
  {
    gaa_assert(list.size() >= 2);
    auto rows = list[0].coord.rows();
    for (auto const &img : list) {
      gaa_assert(is_coordinates2(img.coord));
      gaa_assert(img.coord.rows() == rows);
    }
  }

  GAA_ARG_OR(args, number_of_iterations, 50);
  GAA_ARG_OR(args, converge_threshold, 1e-5);
  GAA_ARG_OR(args, collinearity_condition_solver, cc_solve_all);

  Space_intersection_ols_result result;
  result.coord = space_intersection(list[0], list[1]);
  Mat &coord = result.coord;

  while (number_of_iterations-- > 0) {
    bool is_converged = true;

    for (long int row = 0; row != coord.rows(); ++row) {
      Mat coeff(2 * list.size(), 3), residual(2 * list.size(), 1);
      for (size_t i = 0uz; i != list.size(); ++i) {
        auto const &meta = list[i];
        Mat const &img = list[i].coord.row(i);
        EOrient const &eo = meta.eo;
        IOrient const &io = meta.io;

        Mat const rotate = eo2rotation(eo), &obj = coord.row(i),
                  isp = aux2isp(obj2aux(obj, eo), rotate),
                  img_calc = isp2img(isp, io);
        Mat dxy = img - img_calc;
        Collinearity_condition cc =
            collinearity_condition_solver({.x = img_calc(0, 0),
                                           .y = img_calc(0, 1),
                                           .f = io.f,
                                           .h = io.f * io.m,
                                           .k = eo.k,
                                           .w = eo.w,
                                           .z = isp(0, 2),
                                           .rot = rotate

            });

        Mat l(dxy.rows() * 2, 1);
        for (auto li = 0; li != dxy.rows(); ++li) {
          l(2 * li) = dxy(li, 0);
          l(2 * li + 1) = dxy(li, 1);
        }

        coeff.block(2 * i, 0, 2, 3) << -cc.leftCols(3);
        residual.block(2 * i, 0, 2, 1) << l;
      }

      Mat const correction = ols_solve_ALP(coeff, residual);

      if (std::abs(correction(0, 0)) < converge_threshold &&
          std::abs(correction(1, 0)) < converge_threshold &&
          std::abs(correction(2, 0)) < converge_threshold) {
        result.info = Info::Iterative_Solve_Success;
        const Mat N = cholesky_inverse(coeff.transpose() * coeff);
        result.rmse =
            median_error_VP(coeff * correction - residual, list.size() * 2, 3);
        result.sigma = result.rmse * N.cwiseSqrt();
      } else {
        is_converged = false;
      }

      coord(row, 0) += correction(0, 0);
      coord(row, 1) += correction(1, 0);
      coord(row, 2) += correction(2, 0);
    }
    if (is_converged) {
      break;
    }
  }

  return result;
}
} // namespace gaa