#include <print>

#include <matplot/matplot.h>

#include <gaa/geometry/primitive/point_set.hpp>

using Point_2 = gaa::Point_2<double>;
using Point_set_2 = gaa::Point_set<Point_2>;

int main() {
  Point_set_2 point_set;
  auto name_map = point_set.add_property_map<std::string>("name");
  auto const &point_map = point_set.point_map();

  for (int i = 0; i != 10; ++i) {
    auto idx = point_set.push_back(Point_2(i, i));
    boost::put(name_map, idx, std::format("P_{}", i));
  }

  std::println("====================== raw");
  for (auto i : point_set.make_point_range()) {
    Point_2 const &p = boost::get(point_map, i);
    std::println("{}: .x = {}, .y = {}", name_map[i], p.x(), p.y());
  }

  std::println("====================== remove, insert and views");
  point_set.remove(4);
  point_set.push_back(Point_2(69, 69));

  for (auto i : point_set.make_point_range() | std::views::drop(2) |
                    std::views::take(6) | std::views::stride(2)) {
    Point_2 const &p = boost::get(point_map, i);
    std::println("{}: .x = {}, .y = {}", name_map[i], p.x(), p.y());
  }

  std::println("====================== remove");
  point_set.remove(1);
  point_set.remove(2);
  point_set.remove(3);
  for (auto i : point_set.make_point_range()) {
    Point_2 const &p = boost::get(point_map, i);
    std::println("{}: .x = {}, .y = {}", name_map[i], p.x(), p.y());
  }
  std::println("size = {}", point_set.size());
  std::println("num points = {}", point_set.num_points());

  std::println("====================== garbage collect");
  point_set.garbage_collect();
  for (auto i : point_set.make_point_range()) {
    Point_2 const &p = boost::get(point_map, i);
    std::println("{}: .x = {}, .y = {}", name_map[i], p.x(), p.y());
  }
  std::println("size = {}", point_set.size());
  std::println("num points = {}", point_set.num_points());

  {
    namespace plt = matplot;
    auto x = point_set.x();
    auto y = point_set.y();
    plt::plot(x, y, "-o");
    plt::show();
  }
}