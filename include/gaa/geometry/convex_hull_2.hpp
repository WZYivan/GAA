#pragma once

#include <gaa/core/math.hpp>
#include <gaa/geometry/primitive/point_set.hpp>

namespace gaa {
template <class PointRange, class PointMap>
PointRange convex_hull_2(PointRange const &range, PointMap const &pm) {
  gaa_assert(range.size() > 2, "Need at least 3 points for convex hull");

  using point_type = typename PointMap::value_type;

  PointRange sorted = range;
  std::ranges::sort(sorted, [&pm](auto x, auto y) {
    point_type const &a = boost::get(pm, x);
    point_type const &b = boost::get(pm, y);
    return a.x() < b.x() || (a.x() == b.x() && a.y() < b.y());
  });

  auto last =
      std::unique(sorted.begin(), sorted.end(), [&pm](auto lhs, auto rhs) {
        point_type const &a = boost::get(pm, lhs);
        point_type const &b = boost::get(pm, rhs);
        return a.x() == b.x() && a.y() == b.y();
      });
  sorted.erase(last, sorted.end());

  PointRange lower, upper;

  for (auto i : sorted) {
    while (lower.size() >= 2) {
      point_type const &prev = boost::get(pm, lower[lower.size() - 2]);
      point_type const &last = boost::get(pm, lower.back());
      point_type const &cur = boost::get(pm, i);
      if (cross_product(prev, last, cur) <= 0) {
        lower.pop_back();
      } else {
        break;
      }
    }
    lower.push_back(i);
  }

  for (auto i : sorted | std::views::reverse) {
    while (upper.size() >= 2) {
      point_type const &prev = boost::get(pm, upper[upper.size() - 2]);
      point_type const &last = boost::get(pm, upper.back());
      point_type const &cur = boost::get(pm, i);
      if (cross_product(prev, last, cur) <= 0) {
        upper.pop_back();
      } else {
        break;
      }
    }
    upper.push_back(i);
  }

  lower.pop_back();
  upper.pop_back();
  lower.insert(lower.end(), upper.begin(), upper.end());

  return lower;
}
} // namespace gaa