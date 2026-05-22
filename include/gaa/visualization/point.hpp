#pragma once

#include <ranges>

#include <gaa/geometry/primitive/point.hpp>
#include <gaa/wrap/matplot++/plot.hpp>

namespace gaa::plt {
template <class PointRange, class PointMap, class... Args>
decltype(auto) draw_points(PointRange const &r, PointMap const &pts,
                           Args &&...args) {
  using point_type = typename PointMap::value_type;
  using coord_type = typename Point_traits<point_type>::coordinate_type;

  auto x = r |
           std::views::transform([&pts](auto idx) { return pts[idx].x(); }) |
           std::ranges::to<std::vector<coord_type>>();

  auto y = r |
           std::views::transform([&pts](auto idx) { return pts[idx].y(); }) |
           std::ranges::to<std::vector<coord_type>>();

  return plt::scatter(x, y, args...);
}
} // namespace gaa::plt