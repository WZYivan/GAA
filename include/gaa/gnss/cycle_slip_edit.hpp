#pragma once

#include <span>

#include <gaa/core/kw.hpp>
#include <gaa/core/math.hpp>

namespace gaa {

struct Cycle_detect_result {
  std::vector<Interval> intervals;
  std::vector<double> cycle_slips;
  //   std::vector<double> confidence;
};

extern Cycle_detect_result diff_detect(std::span<double const> vals,
                                       int degree);
} // namespace gaa