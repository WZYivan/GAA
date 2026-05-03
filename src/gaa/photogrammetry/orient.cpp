#include <gaa/core/Eigen/utils.hpp>
#include <gaa/core/keywords.hpp>
#include <gaa/photogrammetry/orient.hpp>

namespace gaa {

EOrient io2eo(IOrient const &io, Mat const &obj) {
  gaa_assert(is_coordinates3(obj));
  return EOrient{.x = obj.col(0).mean(),
                 .y = obj.col(1).mean(),
                 .z = obj.col(2).mean() + io.f * io.m,
                 .p = Radian(0.0),
                 .w = Radian(0.0),
                 .k = Radian(0.0)};
}

Mat eo2rotation(EOrient const &eo) {
  return y_rotation(eo.p.value()) * x_rotation(eo.w.value()) *
         z_rotation(eo.k.value());
}

} // namespace gaa