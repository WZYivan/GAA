#include <gaa/photogrammetry/trans.hpp>
#include <gaa/wrap/Eigen/utils.hpp>

namespace gaa {

Mat obj2aux(Mat const &obj, EOrient const &eo) {
  gaa_assert(is_coordinates3(obj));
  return translate(obj, eo.x, eo.y, eo.z);
};

Mat aux2isp(Mat const &aux, Mat const &rot) {
  gaa_assert(is_coordinates3(aux));
  return rotate_CR(aux, rot);
}

Mat isp2img(Mat const &isp, IOrient const &io) {
  gaa_assert(is_coordinates3(isp));
  Mat img(isp.rows(), 2);
  const auto &XBar = isp.col(0).array(), &YBar = isp.col(1).array(),
             &ZBar = isp.col(2).array();
  const double f = io.f;
  img.col(0) = -f * (XBar / ZBar).matrix(); // x
  img.col(1) = -f * (YBar / ZBar).matrix(); // y
  return img;
}

Mat obj2img(Mat const &obj, EOrient const &eo, Mat const &rot,
            IOrient const &io) {
  /*
    obj |>
    obj2aux(eo) |>
    aux2isp(rot) |>
    isp2img(io)
  */
  return isp2img(aux2isp(obj2aux(obj, eo), rot), io);
}

Mat isp2aux(Mat const &isp, Mat const &rot) {
  gaa_assert(is_coordinates3(isp));
  return rotate_CR(isp, rot.transpose());
}

Mat aux2obj(Mat const &aux, EOrient const &eo) {
  gaa_assert(is_coordinates3(aux));
  return translate(aux, -eo.x, -eo.y, -eo.z);
}

} // namespace gaa