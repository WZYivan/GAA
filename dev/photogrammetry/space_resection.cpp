#include <gaa/core/math.hpp>
#include <gaa/photogrammetry/space_resection.hpp>

int main() {
  gaa::Mat img(4, 2), obj(4, 3), all(4, 5);
  all << -86.15, -68.99, 36589.41, 25273.32, 2195.17, -53.40, 82.21, 37631.08,
      31324.51, 728.69, -14.78, -76.63, 39100.97, 24934.98, 2386.50, 10.46,
      64.43, 40426.54, 30319.81, 757.31;
  img = all.leftCols(2);
  obj = all.rightCols(3);
  img /= 1000;

  gaa::IOrient io{.x = 0, .y = 0, .f = 153.24 / 1000, .m = 50000};

  auto result = gaa::space_resection(io, img, obj);

  gaa_assert(result.info == gaa::Info::Iterative_Solve_Success);

  gaa::Mat img_inv = gaa::obj2img(obj, result.eo, result.rot, io);

  for (int r = 0; r != 4; ++r) {
    for (int c = 0; c != 2; ++c) {
      gaa_assert(gaa::within_abs(img(r, c) - img_inv(r, c), 0, 1e-3));
    }
  }
}