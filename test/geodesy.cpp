#include <gaa/core/math.hpp>
#include <gaa/geodesy/ellipsoids.hpp>
#include <gaa/geodesy/project.hpp>
#include <gaa/geodesy/solve.hpp>
#include <numbers>
#include <print>

#define BOOST_TEST_MODULE GeodesyTest
#if !(GAA_USE_PRIVATE_3RD)
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#else
#include <boost/test/included/unit_test.hpp>
#endif

double rad2sec(double rad) { return rad * 180.0 / std::numbers::pi * 3600.0; }

BOOST_TEST_DONT_PRINT_LOG_VALUE(gaa::Ellipsoid)
BOOST_TEST_DONT_PRINT_LOG_VALUE(gaa::Latitude)
BOOST_TEST_DONT_PRINT_LOG_VALUE(gaa::Longitude)

BOOST_AUTO_TEST_SUITE(EllipsoidSuite)

BOOST_AUTO_TEST_CASE(ellipsoid_identity) {
  BOOST_TEST(gaa::cgcs2000 != gaa::wgs84);
}

BOOST_AUTO_TEST_CASE(meridian_arc) {
  for (int i = 1; i <= 15; ++i) { // 原循环 75/5+1 = 16? 原为 1..15
    double B = gaa::deg2rad(i * 5);
    double S = gaa::cgcs2000.meridian_arc_length(B);
    double Bf = gaa::cgcs2000.meridian_arc_bottom_latitude(S);
    // 允许 1e-10 弧度的差异（约 0.0002 秒）
    BOOST_TEST(gaa::rad2sec(std::abs(B - Bf)) < 0.0002);
  }
}

BOOST_AUTO_TEST_SUITE_END()

// 测试套件：高斯投影
BOOST_AUTO_TEST_SUITE(GaussKrugerSuite)

BOOST_AUTO_TEST_CASE(forward_backward) {
  auto const &ellipsoid = gaa::cgcs2000;
  auto const &project = gaa::gauss_kruger.project(6.0);
  double L = gaa::deg2rad(115);
  for (int i = 1; i < 90; ++i) {
    double B = gaa::deg2rad(i);
    auto gc = ellipsoid.coordinate(B, L);
    auto igc = gc >> project >> ellipsoid;
    double rb = rad2sec((gc.latitude - igc.latitude).value());
    double rl = rad2sec((gc.longitude - igc.longitude).value());
    BOOST_TEST(rb < 1e-3); // 容差 0.001 秒
    BOOST_TEST(rl < 1e-3);
  }
  double B45 = gaa::deg2rad(45);
  auto gc45 = ellipsoid.coordinate(B45, L) >> project >> gaa::geodetic_cast >>
              project(23) >> project(20) >> gaa::geodetic_cast(ellipsoid);
  BOOST_TEST((gc45.latitude - gaa::Latitude(B45)).value() < 1e-6,
             boost::test_tools::tolerance(1e-6));
  BOOST_TEST((gc45.longitude - gaa::Longitude(L)).value() < 1e-6,
             boost::test_tools::tolerance(1e-6));
}

BOOST_AUTO_TEST_SUITE_END()

// 测试套件：Bessel 解算
BOOST_AUTO_TEST_SUITE(BesselSuite)

BOOST_AUTO_TEST_CASE(bessel_forward_backward) {
  gaa::Latitude B1(gaa::dms(47, 46, 52.647'0));
  gaa::Longitude L1(gaa::dms(35, 49, 36.330'0));
  gaa::Radian A1(gaa::dms(44, 12, 13.664));
  double S = 44'797.282'6;

  auto ri = gaa::cgcs2000.coordinate(B1, L1) >> gaa::bessel(S, A1) >>
            gaa::rbessel(B1, L1, gaa::kw{}.converge_threshold(1e-10));

  double da = rad2sec((A1 - ri.angle).value());
  double ds = S - ri.s;
  BOOST_TEST(da < 1e-2, boost::test_tools::tolerance(1e-2));
  BOOST_TEST(ds < 1e-2, boost::test_tools::tolerance(1e-2));
}

BOOST_AUTO_TEST_CASE(bessel_inverse) {
  gaa::Latitude B1(gaa::dms(47, 46, 52.647'0));
  gaa::Latitude B2(gaa::dms(48, 4, 9.638'4));
  gaa::Longitude L1(gaa::dms(35, 49, 36.330'0));
  gaa::Longitude L2(gaa::dms(36, 14, 45.050'5));

  auto ri = gaa::cgcs2000.coordinate(B1, L1) >> gaa::rbessel(B2, L2);
  auto rf = ri >> gaa::bessel(B1, L1);

  double dl = rad2sec((rf.longitude - L2).value());
  double db = rad2sec((rf.latitude - B2).value());
  double da = rad2sec((rf.rangle - ri.rangle).value());
  BOOST_TEST(dl < 1e-2, boost::test_tools::tolerance(1e-2));
  BOOST_TEST(db < 1e-3, boost::test_tools::tolerance(1e-3));
  BOOST_TEST(da < 1.0, boost::test_tools::tolerance(1.0)); // 角度差异允许 1 秒
}

BOOST_AUTO_TEST_SUITE_END()

// 测试套件：高斯勒让德解算
BOOST_AUTO_TEST_SUITE(GaussLemmaSuite)

BOOST_AUTO_TEST_CASE(gauss_lemma_forward_backward) {
  gaa::Latitude B1(gaa::dms(47, 46, 52.647'0));
  gaa::Longitude L1(gaa::dms(35, 49, 36.330'0));
  gaa::Radian A1(gaa::dms(44, 12, 13.664));
  double S = 44'797.282'6;

  auto ri = gaa::cgcs2000.coordinate(B1, L1) >> gaa::gauss_lemma(S, A1) >>
            gaa::rgauss_lemma(B1, L1);

  double da = rad2sec((A1 - ri.angle).value());
  double ds = S - ri.s;
  BOOST_TEST(da < 10.0); // 允许较大误差（原代码 10 秒）
  BOOST_TEST(ds < 1.0);  // 允许 1 米
}

BOOST_AUTO_TEST_CASE(gauss_lemma_inverse) {
  gaa::Latitude B1(gaa::dms(47, 46, 52.647'0));
  gaa::Latitude B2(gaa::dms(48, 4, 9.638'4));
  gaa::Longitude L1(gaa::dms(35, 49, 36.330'0));
  gaa::Longitude L2(gaa::dms(36, 14, 45.050'5));

  auto ri = gaa::cgcs2000.coordinate(B1, L1) >> gaa::rgauss_lemma(B2, L2);
  auto rf = ri >> gaa::gauss_lemma(B1, L1);

  double dl = rad2sec((rf.longitude - L2).value());
  double db = rad2sec((rf.latitude - B2).value());
  double da = rad2sec((rf.rangle - ri.rangle).value());
  BOOST_TEST(dl < 1e-3, boost::test_tools::tolerance(1e-3));
  BOOST_TEST(db < 1e-1, boost::test_tools::tolerance(1e-1));
  BOOST_TEST(da < 1e-3, boost::test_tools::tolerance(1e-3));
}

BOOST_AUTO_TEST_SUITE_END()

// 测试套件：混合方法（交叉验证）
BOOST_AUTO_TEST_SUITE(MixedSuite)

BOOST_AUTO_TEST_CASE(bessel_forward_gauss_inverse) {
  gaa::Latitude B1(gaa::dms(47, 46, 52.647'0));
  gaa::Longitude L1(gaa::dms(35, 49, 36.330'0));
  gaa::Radian A1(gaa::dms(44, 12, 13.664));
  double S = 44'797.282'6;

  auto ri = gaa::cgcs2000.coordinate(B1, L1) >> gaa::bessel(S, A1) >>
            gaa::rgauss_lemma(B1, L1);

  double da = rad2sec((A1 - ri.angle).value());
  double ds = S - ri.s;
  BOOST_TEST(da < 2);
  BOOST_TEST(ds < 1e-2);
}

BOOST_AUTO_TEST_CASE(gauss_forward_bessel_inverse) {
  gaa::Latitude B1(gaa::dms(47, 46, 52.647'0));
  gaa::Latitude B2(gaa::dms(48, 4, 9.638'4));
  gaa::Longitude L1(gaa::dms(35, 49, 36.330'0));
  gaa::Longitude L2(gaa::dms(36, 14, 45.050'5));

  auto ri = gaa::cgcs2000.coordinate(B1, L1) >> gaa::rgauss_lemma(B2, L2);
  auto rf = ri >> gaa::bessel(B1, L1);

  double dl = rad2sec((rf.longitude - L2).value());
  double db = rad2sec((rf.latitude - B2).value());
  double da = rad2sec((rf.rangle - ri.rangle).value());
  // 保持原代码的输出（仅作参考）
  std::println("gauss_lemma + rbessel: dl = {}, db = {}, da = {}", dl, db, da);
  // 实际测试断言（可酌情放宽）
  BOOST_TEST(dl < 1e-2); // 可以根据精度调整
  BOOST_TEST(db < 1e-1);
  BOOST_TEST(da < 1e-2);
}

BOOST_AUTO_TEST_SUITE_END()