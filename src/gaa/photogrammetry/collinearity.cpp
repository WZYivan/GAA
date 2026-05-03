#include <gaa/photogrammetry/collinearity.hpp>

namespace gaa {

Collinearity_condition cc_solve_all(Collinearity_condition_param p) {
  double x = p.x, y = p.y, f = p.f, H = p.h;

  double xx = f + std::pow(x, 2) / f, yy = f + std::pow(y, 2) / f,
         xy = x * y / f;
  Collinearity_condition cc;
  cc << -f / H, 0, -x / H, -xx, -xy, y, 0, -f / H, -y / H, -xy, -yy, -x;
  return cc;
}

Collinearity_condition cc_solve_kappa(Collinearity_condition_param p) {
  double x = p.x, y = p.y, f = p.f, H = p.h, k = rad(p.k);

  double xx = f + std::pow(x, 2) / f, yy = f + std::pow(y, 2) / f,
         xy = x * y / f, cosk = std::cos(k), sink = std::sin(k);

  Collinearity_condition cc;
  cc << -f / H * cosk, -f / H * sink, -x / H, -xx * cosk + xy * sink,
      -xy * cosk - xx * sink, y, f / H * sink, -f / H * cosk, -y / H,
      -xy * cosk + yy * sink, -yy * cosk - xy * sink, -x;
  return cc;
}

Collinearity_condition cc_solve_none(Collinearity_condition_param p) {
  double x = p.x, y = p.y, z = p.z, f = p.f, k = rad(p.k), w = rad(p.w);
  Mat const &rotate = p.rot;

  double cosk = std::cos(k), sink = std::sin(k), cosw = std::cos(w),
         sinw = std::sin(w);
  const auto &a = rotate.row(0), &b = rotate.row(1), &c = rotate.row(2);

  Collinearity_condition cc;
  cc << 1 / z * (a(0) * f + a(2) * x), 1 / z * (b(0) * f + b(2) * x),
      1 / z * (c(0) * f + c(2) * x),
      y * sinw - (x / f * (x * cosk - y * sink) + f * cosk) * cosw,
      -f * sink - x / f * (x * sink + y * cosk), y,
      1 / z * (a(1) * f + a(2) * y), 1 / z * (b(1) * f + b(2) * y),
      1 / z * (c(1) * f + c(2) * y),
      -x * sinw - (y / f * (x * cosk - y * sink) - f * sink) * cosw,
      -f * cosk - y / f * (x * sink + y * cosk), -x;
  return cc;
}
} // namespace gaa