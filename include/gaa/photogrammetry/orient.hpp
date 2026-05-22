#pragma once

#include <gaa/wrap/Eigen/pre.hpp>
#include <gaa/wrap/boost/units.hpp>

namespace gaa {
struct IOrient {
  double x;
  double y;
  double f;
  double m;
};

struct EOrient {
  double x;
  double y;
  double z;
  Radian p;
  Radian w;
  Radian k;
};

struct Orientated_image {
  IOrient io;
  EOrient eo;
  Mat coord;
};

extern EOrient io2eo(IOrient const &io, Mat const &obj);
extern Mat eo2rotation(EOrient const &eo);
} // namespace gaa