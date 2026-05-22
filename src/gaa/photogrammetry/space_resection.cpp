#include <cmath>
#include <gaa/photogrammetry/space_resection.hpp>
#include <gaa/wrap/Eigen/utils.hpp>

namespace gaa {
Space_resection_result space_resection(IOrient const &io, Mat const &img,
                                       Mat const &obj, kwargs args) {
  gaa_assert(is_coordinates2(img));
  gaa_assert(is_coordinates3(obj));

  GAA_ARG_OR(args, number_of_iterations, 50);
  GAA_ARG_OR(args, converge_threshold, 1e-5);
  GAA_ARG_OR(args, collinearity_condition_solver, cc_solve_all);

  Space_resection_result result;
  result.info = Info::Unknown_Status;
  result.eo = io2eo(io, obj);
  EOrient &eo = result.eo;

  while (number_of_iterations-- > 0) {
    Mat rot = eo2rotation(eo);
    Mat isp = aux2isp(obj2aux(obj, eo), rot);
    Mat img2 = isp2img(isp, io);
    Mat dxy = img - img2;
    Mat residual(img.rows() * 2, 1);

    for (long int li = 0; li != dxy.rows(); ++li) {
      residual(2 * li) = dxy(li, 0);
      residual(2 * li + 1) = dxy(li, 1);
    }

    Mat coeff(img2.rows() * 2, 6);
    Collinearity_condition_param ccp{.x{},
                                     .y{},
                                     .f = io.f,
                                     .h = io.f * io.m,
                                     .k = eo.k,
                                     .w = eo.w,
                                     .z{},
                                     .rot = rot};
    for (long int pi = 0; pi != img2.rows(); ++pi) {
      ccp.x = img2(pi, 0);
      ccp.y = img2(pi, 1);
      ccp.z = isp(pi, 2);

      Collinearity_condition cc = collinearity_condition_solver(ccp);
      coeff.row(pi * 2) << cc.row(0);
      coeff.row(pi * 2 + 1) << cc.row(1);
    }

    Mat corr = ols_solve_ALP(coeff, residual);

    if (std::abs(corr(3)) < converge_threshold &&
        std::abs(corr(4)) < converge_threshold &&
        std::abs(corr(5)) < converge_threshold) {
      Mat v = coeff * corr - residual;
      Mat n_inv = cholesky_inverse((coeff.transpose() * coeff).eval());
      result.rmse = median_error_VP(v, coeff.rows(), 6);
      result.sigma = result.rmse * n_inv.cwiseSqrt();
      result.img = Mat(img);
      result.rot = Mat(rot);
      Mat &p = result.img;
      for (size_t pc = coeff.rows() / 2, pi = 0uz; pi != pc; ++pi) {
        p(pi, 0) = (p(pi, 0) + v(2 * pi)) * 1000;
        p(pi, 1) = (p(pi, 1) + v(2 * pi + 1)) * 1000;
      }
      result.info = Info::Iterative_Solve_Success;
      break;
    }

    eo.x += corr(0);
    eo.y += corr(1);
    eo.z += corr(2);
    eo.p += rad(corr(3));
    eo.w += rad(corr(4));
    eo.k += rad(corr(5));
  }

  return result;
}
} // namespace gaa