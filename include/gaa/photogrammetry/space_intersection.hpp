#pragma once

#include <span>

#include <gaa/core/enums.hpp>
#include <gaa/core/kw.hpp>
#include <gaa/photogrammetry/orient.hpp>
#include <gaa/photogrammetry/trans.hpp>

namespace gaa {
extern Mat space_intersection(Orientated_image const &img1,
                              Orientated_image const &img2);

struct Space_intersection_ols_result {
  Mat coord;
  Mat sigma;
  double rmse;
  Info info;
};

extern Space_intersection_ols_result
space_intersection_ols(std::span<Orientated_image const> list,
                       kwargs args = {});
} // namespace gaa