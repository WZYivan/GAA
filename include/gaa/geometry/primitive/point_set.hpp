#pragma once

#include <ranges>
#include <set>
#include <vector>

#include <boost/geometry/algorithms/num_points.hpp>
#include <boost/geometry/geometries/multi_point.hpp>

#include <gaa/geometry/primitive/point.hpp>

namespace gaa {
template <class Point> class Point_set {
public:
  using point_type = Point;
  using point_traits = Point_traits<point_type>;
  static_assert(point_traits::is_valid);

  using point_map_type =
      geom::model::multi_point<point_type, std::vector, std::allocator>;
  using wrapped = point_map_type;
  using unwrap_ref = std::add_lvalue_reference_t<wrapped>;
  using unwrap_cref = std::add_lvalue_reference_t<std::add_const_t<wrapped>>;

  using reindex_fn = void (*)(std::any &, Point_range const &);

private:
  point_map_type m_mp;
  std::set<Point_index> m_removed;
  Associative_property_maps_collection m_pm;
  std::map<Associative_property_maps_collection::key_type, reindex_fn>
      m_reindex;

public:
  unwrap_ref unwrap() { return m_mp; }
  unwrap_cref unwrap() const { return m_mp; }

  template <class P>
    requires std::same_as<std::remove_cvref_t<P>, point_type>
  std::size_t push_back(P &&p) {
    if (!m_removed.empty()) {
      auto pos = m_removed.begin();
      Point_index idx = *pos;
      std::swap(m_mp.at(idx), p);
      m_removed.erase(pos);
      return idx;
    } else {
      m_mp.emplace_back(p);
      return this->size() - 1;
    }
  }

  void remove(Point_index idx) {
    gaa_assert(idx < this->size(),
               std::format("index out of range ({} > {})", idx, this->size()));
    m_removed.insert(idx);
  }

  void garbage_collect() {
    std::vector<Point_index> alive;
    for (auto i : make_point_range()) {
      alive.push_back(i);
    }

    for (auto &[name, storage] : m_pm.storage()) {
      m_reindex.at(name)(storage, alive);
    }

    point_map_type new_mp;
    new_mp.reserve(alive.size());
    for (auto i : alive)
      new_mp.push_back(std::move(m_mp[i]));
    m_mp.swap(new_mp);

    m_removed.clear();
  }

  template <class Property>
  decltype(auto) add_property_map(std::string const &name) {
    m_reindex.insert_or_assign(
        name, [](std::any &S, Point_range const &new_range) -> void {
          using Map = std::map<Point_index, Property>;
          Map *origin = std::any_cast<Map>(&S);
          Map new_map;
          for (std::size_t new_idx = 0; new_idx < new_range.size(); ++new_idx) {
            Point_index old_idx = new_range[new_idx];
            auto node = origin->extract(old_idx);
            node.key() = new_idx;
            new_map.insert(std::move(node));
          }
          origin->swap(new_map);
        });
    return m_pm.add<Point_index, Property>(name);
  }

  decltype(auto) erase_property_map(std::string const &name) {
    gaa_assert(this->has_property_map(name));
    m_reindex.erase(name);
    return m_pm.erase(name);
  }

  bool has_property_map(std::string const &name) { return m_pm.contains(name); }

  point_type &at(std::size_t idx) { return m_mp.at(idx); }
  point_type const &at(std::size_t idx) const { return m_mp.at(idx); }
  std::size_t size() const { return m_mp.size(); }
  std::size_t num_points() const { return this->size() - m_removed.size(); }

  Point_range make_point_range() const {
    return std::views::iota(Point_index(0), this->size()) |
           std::views::filter([&](Point_index idx) -> bool {
             return !m_removed.contains(idx);
           }) |
           std::ranges::to<Point_range>();
  }
  point_map_type const &point_map() const { return m_mp; }

  template <template <class> class Container = std::vector>
  Container<typename point_traits::coordinate_type> x() const {
    return m_mp | std::views::transform([](point_type const &p) -> auto {
             return p.x();
           }) |
           std::ranges::to<Container<typename point_traits::coordinate_type>>();
  }
  template <template <class> class Container = std::vector>
  Container<typename point_traits::coordinate_type> y() const {
    return m_mp | std::views::transform([](point_type const &p) -> auto {
             return p.y();
           }) |
           std::ranges::to<Container<typename point_traits::coordinate_type>>();
  }
  template <template <class> class Container = std::vector>
  Container<typename point_traits::coordinate_type> z() const {
    return m_mp | std::views::transform([](point_type const &p) -> auto {
             return p.z();
           }) |
           std::ranges::to<Container<typename point_traits::coordinate_type>>();
  }

  template <template <class> class Container = std::vector>
  Container<typename point_traits::coordinate_type>
  x(Point_range const &r) const {
    return r | std::views::transform([&](Point_index idx) -> auto {
             return boost::get(m_mp, idx).x();
           }) |
           std::ranges::to<Container<typename point_traits::coordinate_type>>();
  }
  template <template <class> class Container = std::vector>
  Container<typename point_traits::coordinate_type>
  y(Point_range const &r) const {
    return r | std::views::transform([&](Point_index idx) -> auto {
             return boost::get(m_mp, idx).y();
           }) |
           std::ranges::to<Container<typename point_traits::coordinate_type>>();
  }
  template <template <class> class Container = std::vector>
  Container<typename point_traits::coordinate_type>
  z(Point_range const &r) const {
    return r | std::views::transform([&](Point_index idx) -> auto {
             return boost::get(m_mp, idx).z();
           }) |
           std::ranges::to<Container<typename point_traits::coordinate_type>>();
  }
};
} // namespace gaa

namespace boost {
template <typename Point>
Point const &
get(::boost::geometry::model::multi_point<Point, std::vector,
                                          std::allocator> const &pm,
    ::gaa::Point_index idx) {
  return pm.at(idx);
}

template <typename Point>
Point &get(::boost::geometry::model::multi_point<Point, std::vector,
                                                 std::allocator> &pm,
           ::gaa::Point_index idx) {
  return pm.at(idx);
}
} // namespace boost