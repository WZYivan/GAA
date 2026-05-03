#pragma once

#include <gaa/core/Eigen/pre.hpp>
#include <gaa/photogrammetry/orient.hpp>

namespace gaa {
extern Mat obj2aux(Mat const &obj, EOrient const &eo);
extern Mat aux2isp(Mat const &aux, Mat const &rot);
extern Mat isp2img(Mat const &isp, IOrient const &io);
extern Mat obj2img(Mat const &obj, EOrient const &eo, Mat const &rot,
                   IOrient const &io);
extern Mat isp2aux(Mat const &isp, Mat const &rot);
extern Mat aux2obj(Mat const &aux, EOrient const &eo);
} // namespace gaa