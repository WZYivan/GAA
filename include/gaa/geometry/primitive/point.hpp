#pragma once

#include <vector>

#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/point_xyz.hpp>

#include <gaa/wrap/boost/geometry.hpp>
#include <gaa/wrap/boost/property_map/associative_property_map.hpp>

namespace gaa {
template <class CoordinateType = double,
          class CoordinateSystem = geom::cs::cartesian>
using Point_2 = geom::model::d2::point_xy<CoordinateType, CoordinateSystem>;

template <class CoordinateType = double,
          class CoordinateSystem = geom::cs::cartesian>
using Point_3 = geom::model::d3::point_xyz<CoordinateType, CoordinateSystem>;

template <class T>
concept Is_Point = std::same_as<geom::point_tag, typename geom::tag<T>::type>;

template <Is_Point Point> struct Point_traits {
  using point_type = Point;
  using coordinate_type = typename geom::traits::coordinate_type<Point>::type;
  using coordinate_system =
      typename geom::traits::coordinate_system<Point>::type;
  constexpr static std::size_t dimension_count =
      geom::traits::dimension<Point>::value;
  constexpr static bool is_valid = true;
};

using Point_index = std::size_t;
using Point_range = std::vector<Point_index>;
template <class Property>
using Point_map =
    typename Associative_property_maps_collection::map_interface_type<
        Point_index, Property>;

template <class Property>
using _point_map_storage =
    typename Associative_property_maps_collection::map_type<Point_index,
                                                            Property>;

template <class Property> struct _make_point_map_result_t {
  _point_map_storage<Property> data;
  Point_map<Property> interface;
  bool good;
};

template <class Property> _make_point_map_result_t<Property> make_point_map() {
  _make_point_map_result_t<Property> R{};
  auto [inter, status] = boost::make_assoc_property_map(R.data);
  R.interface = inter;
  R.good = status;
  return R;
}

extern Point_range make_point_range(Point_index begin, Point_index count,
                                    Point_index interval = 1);

template <Is_Point P>
typename Point_traits<P>::coordinate_type cross_product(P const &a, P const &b,
                                                        P const &c) {
  return (b.x() - a.x()) * (c.y() - a.y()) - (b.y() - a.y()) * (c.x() - a.x());
}
} // namespace gaa