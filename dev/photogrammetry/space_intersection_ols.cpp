#include <iostream>
#include <vector>

#include <gaa/core/math.hpp>
#include <gaa/photogrammetry/orient.hpp>
#include <gaa/photogrammetry/space_intersection.hpp>
#include <gaa/photogrammetry/trans.hpp>
#include <gaa/wrap/Eigen/utils.hpp>

using namespace gaa;

int main() {
  double f = 150.0 / 1000.0;
  double m = 50000;
  EOrient eo_left{4999.770168,     4999.728897,     2000.002353,
                  rad(0.00021500), rad(0.02906441), rad(0.09524706)},
      eo_right{5896.828551,    5070.244316,    2030.443250,
               0.01443355_rad, 0.04601826_rad, 0.11046904_rad};
  IOrient io_left{.f = f, .m = m}, io_right{.f = f, .m = m};
  Mat img(5, 4);
  img << 51.758, 80.555, -39.953, 78.463, 14.618, -0.231, -76.006, 0.036, 49.88,
      -0.782, -42.201, -1.022, 86.14, -1.346, -7.706, -2.112, 48.035, -79.962,
      -44.438, -79.736;
  img /= 1000.0;
  Orientated_image img1{io_left, eo_left, img.leftCols(2)},
      img2{io_right, eo_right, img.rightCols(2)};
  std::vector<Orientated_image> params{img1, img2};

  auto result = space_intersection_ols(params);
  Mat img_left_inv =
      obj2img(result.coord, eo_left, eo2rotation(eo_left), io_left);
  Mat img_right_inv =
      obj2img(result.coord, eo_right, eo2rotation(eo_right), io_right);
  Mat img_inv(5, 4);
  img_inv.leftCols(2) = img_left_inv;
  img_inv.rightCols(2) = img_right_inv;

  std::cout << img.format(gaa::EigenFmt::python) << "\n"
            << img_inv.format(gaa::EigenFmt::python) << "\n";

  for (int r = 0; r != 5; ++r) {
    for (int c = 0; c != 4; ++c) {
      gaa_assert(within_abs(img(r, c) - img_inv(r, c), 0, 1e-3));
    }
  }
}