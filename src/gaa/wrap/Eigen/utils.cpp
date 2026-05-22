#include <gaa/wrap/Eigen/utils.hpp>

namespace gaa {
Mat x_rotation(double omega) {
  const double sinw = std::sin(omega), cosw = std::cos(omega);
  Mat Rw(3, 3);
  Rw << 1, 0, 0, 0, cosw, -sinw, 0, sinw, cosw;
  return Rw;
}

Mat y_rotation(double phi) {
  const double sinp = std::sin(phi), cosp = std::cos(phi);
  Mat Rp(3, 3);
  Rp << cosp, 0, -sinp, 0, 1, 0, sinp, 0, cosp;
  return Rp;
}

Mat z_rotation(double kappa) {
  const double sink = std::sin(kappa), cosk = std::cos(kappa);
  Mat Rk(3, 3);
  Rk << cosk, -sink, 0, sink, cosk, 0, 0, 0, 1;
  return Rk;
}

namespace EigenFmt {
Eigen::IOFormat const python(Eigen::FullPrecision, 0, ", ", ";\n", "[", "]",
                             "[", "]"),
    csv(Eigen::FullPrecision, 0, ", ", "\n", "", "", "", "");
}
} // namespace gaa