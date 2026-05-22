#pragma once

#include <gaa/core/message.hpp>
#include <gaa/core/units.hpp>
#include <gaa/geodesy/ellipsoid/entity.hpp>
#include <gaa/geodesy/ellipsoid/ref.hpp>

namespace gaa {
struct Geodetic_coordinate {
  Latitude latitude;
  Longitude longitude;
  double height;
  Ellipsoid_ref ellipsoid = null_ellipsoid;

  ~Geodetic_coordinate() = default;
  Geodetic_coordinate(double lat, double lon, Ellipsoid const &e)
      : latitude(lat), longitude(lon), height(0.0), ellipsoid(e) {}
  Geodetic_coordinate(Latitude lat, Longitude lon, Ellipsoid const &e)
      : latitude(lat), longitude(lon), height(0.0), ellipsoid(e) {}
  Geodetic_coordinate(Latitude lat, Longitude lon, double h, Ellipsoid const &e)
      : latitude(lat), longitude(lon), height(h), ellipsoid(e) {}
};

template <class T> struct Geodetic_coordinate_caster {
  static void cast(T const &) { static_assert(false, "must be specialized"); }
  static void cast_to(T const &, Ellipsoid const &) {
    static_assert(false, "must be specialized");
  }
};

template <class T>
concept Convertible_To_Geodetic_Coordinate = requires {
  {
    Geodetic_coordinate_caster<T>::cast(std::declval<T>())
  } -> std::convertible_to<Geodetic_coordinate>;
  {
    Geodetic_coordinate_caster<T>::cast_to(std::declval<T>(),
                                           std::declval<Ellipsoid>())
  } -> std::convertible_to<Geodetic_coordinate>;
};

struct _geodetic_coordinate_cast_to_fn {
  Ellipsoid const &ellipsoid;

  _geodetic_coordinate_cast_to_fn(Ellipsoid const &e) : ellipsoid(e) {}

  ~_geodetic_coordinate_cast_to_fn() = default;

  template <Convertible_To_Geodetic_Coordinate T>
  Geodetic_coordinate operator()(T const &s) const {
    return Geodetic_coordinate_caster<T>::cast_to(s, ellipsoid);
  }
};

struct _geodetic_coordinate_cast_fn {

  template <Convertible_To_Geodetic_Coordinate T>
    requires requires { !std::same_as<T, Ellipsoid>; }
  Geodetic_coordinate operator()(T const &s) const {
    return Geodetic_coordinate_caster<T>::cast(s);
  }

  _geodetic_coordinate_cast_to_fn operator()(Ellipsoid const &e) const {
    return _geodetic_coordinate_cast_to_fn(e);
  }
};

constexpr _geodetic_coordinate_cast_fn geodetic_cast = {};

template <class T>
GAA_weak_channel((Geodetic_coordinate), (T), const &t,
                 (_geodetic_coordinate_cast_fn), const &fn) {
  return fn(t);
}

template <class T>
GAA_weak_channel((Geodetic_coordinate), (T), const &t,
                 (_geodetic_coordinate_cast_to_fn), const &fn) {
  return fn(t);
}
} // namespace gaa