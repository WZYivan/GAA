#define BOOST_TEST_MODULE PhotogrammetryTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/included/unit_test.hpp>

#include <gaa/core/math.hpp>
#include <gaa/photogrammetry/orient.hpp>
#include <gaa/photogrammetry/space_intersection.hpp>
#include <gaa/photogrammetry/space_resection.hpp>
#include <gaa/photogrammetry/trans.hpp>

using namespace gaa;

BOOST_TEST_DONT_PRINT_LOG_VALUE(Mat)
BOOST_TEST_DONT_PRINT_LOG_VALUE(Info)

BOOST_AUTO_TEST_SUITE(PhotogrammetrySuite)

BOOST_AUTO_TEST_CASE(space_intersection_test) {
  double f = 150.0 / 1000.0;
  double m = 50000;
  EOrient eo_left{4999.770168,     4999.728897,     2000.002353,
                  rad(0.00021500), rad(0.02906441), rad(0.09524706)};
  EOrient eo_right{5896.828551,    5070.244316,    2030.443250,
                   0.01443355_rad, 0.04601826_rad, 0.11046904_rad};
  IOrient io_left{.f = f, .m = m}, io_right{.f = f, .m = m};
  Mat img(5, 4);
  img << 51.758, 80.555, -39.953, 78.463, 14.618, -0.231, -76.006, 0.036, 49.88,
      -0.782, -42.201, -1.022, 86.14, -1.346, -7.706, -2.112, 48.035, -79.962,
      -44.438, -79.736;
  img /= 1000.0;
  Orientated_image img1{io_left, eo_left, img.leftCols(2)};
  Orientated_image img2{io_right, eo_right, img.rightCols(2)};
  auto result = space_intersection(img1, img2);
  Mat img_left_iov = obj2img(result, eo_left, eo2rotation(eo_left), io_left);
  Mat img_right_iov =
      obj2img(result, eo_right, eo2rotation(eo_right), io_right);
  Mat img_inv(5, 4);
  img_inv.leftCols(2) = img_left_iov;
  img_inv.rightCols(2) = img_right_iov;

  for (int r = 0; r != 5; ++r) {
    for (int c = 0; c != 4; ++c) {
      BOOST_TEST(within_abs(img(r, c) - img_inv(r, c), 0, 1e-3));
    }
  }
}

BOOST_AUTO_TEST_CASE(space_resection_test) {
  Mat img(4, 2), obj(4, 3), all(4, 5);
  all << -86.15, -68.99, 36589.41, 25273.32, 2195.17, -53.40, 82.21, 37631.08,
      31324.51, 728.69, -14.78, -76.63, 39100.97, 24934.98, 2386.50, 10.46,
      64.43, 40426.54, 30319.81, 757.31;
  img = all.leftCols(2);
  obj = all.rightCols(3);
  img /= 1000;

  IOrient io{.x = 0, .y = 0, .f = 153.24 / 1000, .m = 50000};

  auto result = space_resection(io, img, obj);

  BOOST_TEST(result.info == Info::Iterative_Solve_Success);

  Mat img_inv = obj2img(obj, result.eo, result.rot, io);

  for (int r = 0; r != 4; ++r) {
    for (int c = 0; c != 2; ++c) {
      BOOST_TEST(within_abs(img(r, c) - img_inv(r, c), 0, 1e-3));
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()