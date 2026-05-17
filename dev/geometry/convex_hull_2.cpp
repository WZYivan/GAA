#include <print>

#include <gaa/geometry/convex_hull_2.hpp>
#include <gaa/visiualization/linestring.hpp>
#include <gaa/visiualization/point.hpp>

namespace plt = gaa::plt;

using Point_2 = gaa::Point_2<>;
using Point_set_2 = gaa::Point_set<Point_2>;

int main() {
  Point_set_2 pts;

  pts.push_back(Point_2(10, 0));
  pts.push_back(Point_2(10, 0));
  pts.push_back(Point_2(0, 10));
  pts.push_back(Point_2(1, 1));
  pts.push_back(Point_2(3, 4));
  pts.push_back(Point_2(0, 0));
  pts.push_back(Point_2(10, 10));
  pts.push_back(Point_2(2, 6));

  auto point_range = pts.make_point_range();
  auto point_map = pts.point_map();

  gaa::Convex_hull_2 convex(point_range, point_map);
  auto const &convex_pts = convex.range();

  plt::hold(plt::on);

  auto ps2 = plt::draw_points(point_range, point_map);
  ps2->marker_style(plt::line_spec::marker_style::diamond).marker_color("r");

  auto cvx = plt::draw_linestring(convex_pts, point_map);
  cvx->marker_style(plt::line_spec::marker_style::downward_pointing_triangle)
      .marker_size(20)
      .marker_color("b")
      .line_width(5);

  plt::show();
}