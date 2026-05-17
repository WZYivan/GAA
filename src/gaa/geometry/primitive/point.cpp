#include <ranges>

#include <gaa/geometry/primitive/point.hpp>

namespace gaa {
Point_range make_point_range(Point_index begin, Point_index count,
                             Point_index interval) {
  return std::views::iota(Point_index(0), count) |
         std::views::transform([=](auto i) { return begin + i * interval; }) |
         std::ranges::to<Point_range>();
}
} // namespace gaa