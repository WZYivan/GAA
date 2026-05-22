#pragma once

#include <numbers>
#include <tuple>

#include <gaa/core/keywords.hpp>
#include <gaa/core/message.hpp>
#include <gaa/geodesy/coord/fwd.hpp>
#include <gaa/geodesy/coord/geodetic.hpp>
#include <gaa/geodesy/ellipsoid/entity.hpp>
#include <gaa/geodesy/ellipsoid/ref.hpp>
#include <gaa/wrap/boost/units.hpp>

namespace gaa {
class Ellipsoid;
struct _ellipsoid_data_section_t;

class _ellipsoid_geometry_access_t {
private:
  _ellipsoid_data_section_t const &m_data;

public:
  _ellipsoid_geometry_access_t(_ellipsoid_data_section_t const &d)
      : m_data(d) {}

  double a() const;
  double b() const;
  double c() const;
  double alpha() const;
  double e1_2() const;
  double e2_2() const;
};

class _ellipsoid_gravity_access_t {
private:
  _ellipsoid_data_section_t const &m_data;

public:
  _ellipsoid_gravity_access_t(_ellipsoid_data_section_t const &d) : m_data(d) {}

  double M() const;
  double GM() const;
  double p() const;
  double omega() const;
};

struct _ellipsoid_project_accept_proxy_t {
  Ellipsoid const &ellipsoid;

  ~_ellipsoid_project_accept_proxy_t() = default;

  _ellipsoid_project_accept_proxy_t(Ellipsoid const &e) : ellipsoid(e) {}
};

struct _ellipsoid_project_send_package_t {
  Geodetic_coordinate const &geodetic_coordinate;

  _ellipsoid_project_send_package_t(Geodetic_coordinate const &gc)
      : geodetic_coordinate(gc) {}

  ~_ellipsoid_project_send_package_t() = default;
};

struct Latitude_auxiliary_constants {
  double t, nu_2, w, v;

  ~Latitude_auxiliary_constants() = default;
  Latitude_auxiliary_constants(Latitude latitude, Ellipsoid const &ellipsoid);
};

class Ellipsoid {
public:
  using data_type = _ellipsoid_data_section_t;

  enum class Identifier { CGCS2000, WGS84, Krassovsky, EmptyPlaceHolder };

private:
  data_type const &m_data;
  Identifier m_identifier;

public:
  Ellipsoid(data_type const &data, Identifier id)
      : m_data(data), m_identifier(id) {}
  ~Ellipsoid() = default;

  Geodetic_coordinate coordinate(double lat, double lon) const;
  Geodetic_coordinate coordinate(Latitude lat, Longitude lon) const;

  bool operator==(Ellipsoid const &other) const;
  bool operator!=(Ellipsoid const &other) const;
  bool is_null() const;

  std::tuple<double, double, double>
  principle_curvature_radius(double lat) const;
  double meridian_arc_length(double lat) const;
  double meridian_arc_bottom_latitude(double len) const;
  Latitude_auxiliary_constants lat_aux(Latitude lat) const;

  data_type const &data() const;
  _ellipsoid_geometry_access_t geometry() const;
  _ellipsoid_gravity_access_t gravity() const;
};

template <class T>
concept Reference_To_Ellipsoid = requires(T t) {
  { t.ellipsoid } -> std::convertible_to<Ellipsoid>;
} || requires(T t) {
  { t.ellipsoid() } -> std::convertible_to<Ellipsoid>;
};
} // namespace gaa
#define GAA_latitude_assert(LATITUDE)                                          \
  gaa_assert(std::abs(LATITUDE) <= std::numbers::pi / 2.0,                     \
             "Latitude(rad) out of range [-90, 90] deg")
#define GAA_longitude_assert(LONGITUDE)                                        \
  gaa_assert(std::abs(LONGITUDE) <= std::numbers::pi,                          \
             "Latitude(rad) out of range [-180, 180] deg")
#define GAA_azimuth_assert(AZIMUTH)                                            \
  gaa_assert((AZIMUTH) <= std::numbers::pi * 2 && (AZIMUTH) >= 0,              \
             "Azimuth(rad) out of range [0, 360] deg")
#define GAA_distance_assert(DISTANCE)                                          \
  gaa_assert((DISTANCE) >= 0, "Distance(m) out of range [0, +inf]")
