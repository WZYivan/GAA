#pragma once

#include <ranges>

#include <gaa/geometry/primitive/point.hpp>
#include <gaa/wrap/matplot++/plot.hpp>

namespace gaa::plt {
template <class PointRange, class PointMap, class... Args>
decltype(auto) draw_linestring(PointRange const &r, PointMap const &pts,
                               bool closed, Args &&...args) {
  using point_type = typename PointMap::value_type;
  using coord_type = typename Point_traits<point_type>::coordinate_type;

  auto x = r |
           std::views::transform([&pts](auto idx) { return pts[idx].x(); }) |
           std::ranges::to<std::vector<coord_type>>();

  auto y = r |
           std::views::transform([&pts](auto idx) { return pts[idx].y(); }) |
           std::ranges::to<std::vector<coord_type>>();

  if (!closed) {
    *std::back_inserter(x) = *x.begin();
    *std::back_inserter(y) = *y.begin();
  }

  return plt::plot(x, y, args...);
}

template <class PointRange, class PointMap, class... Args>
decltype(auto) draw_linestring(PointRange const &r, PointMap const &pts,
                               Args &&...args) {
  return draw_linestring(r, pts, false, std::forward<Args>(args)...);
}
} // namespace gaa::plt