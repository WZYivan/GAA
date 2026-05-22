#pragma once

#include <gaa/core/enums.hpp>
#include <gaa/core/kw.hpp>
#include <gaa/photogrammetry/collinearity.hpp>
#include <gaa/photogrammetry/orient.hpp>
#include <gaa/photogrammetry/trans.hpp>

namespace gaa {
struct Space_resection_result {
  Mat rot;
  Mat sigma;
  Mat img;
  EOrient eo;
  double rmse;
  Info info;
};

extern Space_resection_result space_resection(IOrient const &io, Mat const &img,
                                              Mat const &obj, kwargs args = {});
} // namespace gaa