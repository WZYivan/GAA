#include <cmath>
#include <tuple>

#include <gaa/core/math.hpp>
#include <gaa/geodesy/ellipsoids.hpp>

namespace gaa {
struct _ellipsoid_data_section_t {
  struct _geometry_property_t {
    double a, b, c, alpha, e1_2, e2_2;
  };

  struct _gravity_property_t {
    double M, GM, p, omega;
  };

  struct _principle_curvature_radius_coefficient_t {
    double m0, m2, m4, m6, m8, n0, n2, n4, n6, n8;

    static _principle_curvature_radius_coefficient_t
    init(_geometry_property_t const &geo) {
      double a = geo.a, e2 = geo.e1_2;
      double m0 = a * (1 - e2);
      double m2 = 3.0 / 2.0 * e2 * m0;
      double m4 = 5.0 / 4.0 * e2 * m2;
      double m6 = 7.0 / 6.0 * e2 * m4;
      double m8 = 9.0 / 8.0 * e2 * m6;
      double n0 = a;
      double n2 = 1.0 / 2.0 * e2 * n0;
      double n4 = 3.0 / 4.0 * e2 * n2;
      double n6 = 5.0 / 6.0 * e2 * n4;
      double n8 = 7.0 / 8.0 * e2 * n6;
      return {m0, m2, m4, m6, m8, n0, n2, n4, n6, n8};
    }
  };

  struct _quarter_arc_coefficient_t {
    double a0, a2, a4, a6, a8;

    static _quarter_arc_coefficient_t
    init(_principle_curvature_radius_coefficient_t const &coeff) {
      double m0 = coeff.m0, m2 = coeff.m2, m4 = coeff.m4, m6 = coeff.m6,
             m8 = coeff.m8;

      double a0 = deg2rad(m0 + m2 / 2.0 + m4 * 3.0 / 8.0 + m6 * 5.0 / 16.0 +
                          m8 * 35.0 / 128.0),
             a2 = (m2 / 2.0 + m4 / 2.0 + m6 * 15.0 / 32.0 + m8 * 7.0 / 16.0) /
                  2.0,
             a4 = (m4 / 8.0 + m6 * 3.0 / 16.0 + m8 * 7.0 / 32.0) / 4.0,
             a6 = (m6 / 32.0 + m8 / 16.0) / 6.0, a8 = (1. / 128. * m8) / 8.;
      return {a0, a2, a4, a6, a8};
    }
  };

  struct _quarter_arc_bottom_coefficient_t {
    double a0, c1, c2, c3, c4, c5, c6, c7;

    static _quarter_arc_bottom_coefficient_t
    init(_geometry_property_t const &geo) {
      double e2 = geo.e1_2, e4 = std::pow(e2, 2), e6 = std::pow(e2, 3),
             e8 = std::pow(e2, 4), e10 = std::pow(e2, 5), e12 = std::pow(e2, 6),
             e14 = std::pow(e2, 7), e16 = std::pow(e2, 8);

      double C1 = 3. / 8. * e2 + 3. / 16. * e4 + 213. / 2048. * e6 +
                  255. / 4096. * e8 + 20861. / 524288. * e10 +
                  28273. / 1048576. * e12 + 8091605. / 402653184. * e14,
             C2 = 21. / 256. * e4 + 21. / 256. * e6 + 533. / 8192. * e8 +
                  197. / 4096. * e10 + 713639. / 16777216. * e12 +
                  811765. / 16777216. * e14,
             C3 = 151. / 6144. * e6 + 151. / 4096. * e8 +
                  5019. / 131072. * e10 + 26965. / 786432. * e12 +
                  11074043. / 335544320. * e14,
             C4 = 1097. / 131072. * e8 + 1097. / 65536. * e10 +
                  160997. / 10485760. * e12 + 44191. / 10485760 * e14,
             C5 = 8011. / 2621440 * e10 + 8011. / 1048576 * e12 +
                  480757. / 100663296 * e14,
             C6 = 682193. / 25168240. * e12 + 682193. / 83886080. * e14,
             C7 = 36941521. / 14092861440. * e14;

      double a0 = 1 + 3. / 4. * e2 + 45. / 64. * e4 + 175. / 256. * e6 +
                  11025. / 16384. * e8 + 43659. / 65536. * e10 +
                  693693. / 1048576. * e12 + 2760615. / 4194304. * e14 +
                  703956825. / 1073741824. * e16;

      return {a0, C1, C2, C3, C4, C5, C6, C7};
    }
  };

  _geometry_property_t geometry_property;
  _gravity_property_t gravity_property;
  _principle_curvature_radius_coefficient_t
      principle_curvature_radius_coefficient;
  _quarter_arc_coefficient_t quarter_arc_coefficient;
  _quarter_arc_bottom_coefficient_t quarter_arc_bottom_coefficient;

  _ellipsoid_data_section_t(_geometry_property_t const &geo,
                            _gravity_property_t const &gra) {
    geometry_property = geo;
    gravity_property = gra;
    principle_curvature_radius_coefficient =
        _principle_curvature_radius_coefficient_t::init(geo);
    quarter_arc_coefficient = _quarter_arc_coefficient_t::init(
        principle_curvature_radius_coefficient);
    quarter_arc_bottom_coefficient =
        _quarter_arc_bottom_coefficient_t::init(geo);
  }

  ~_ellipsoid_data_section_t() = default;
};

_ellipsoid_data_section_t _krassovsky_data({.a = 6'378'245,
                                            .b = 6'356'863.018'773'047'3,
                                            .c = 6'399'698.901'782'771'0,
                                            .alpha = 1.0 / 298.3,
                                            .e1_2 = 0.006'693'421'622'966,
                                            .e2_2 = 0.006'738'525'414'683},
                                           {}),
    _wgs84_data(
        {.a = 6'378'140,
         .b = 6'356'755.288'157'528'7,
         .c = 6'399'596.651'988'010'5,
         .alpha = 1.0 / 298.257,
         .e1_2 = 0.006'694'384'999'588,
         .e2_2 = 0.006'739'501'819'473},
        {.M = {}, .GM = 3.986'005e14, .p = {}, .omega = 7.292'115'146'7e-5}),
    _cgcs2000_data({.a = 6'378'137,
                    .b = 6'356'752.314'1,
                    .c = 6'399'593.625'9,
                    .alpha = 1 / 298.257'222'101,
                    .e1_2 = 0.006'694'380'022'90,
                    .e2_2 = 0.006'739'496'775'48},
                   {.M = 5.976e24,
                    .GM = 3.986'004'418e14,
                    .p = 5.518,
                    .omega = 7.292115e-5});

Ellipsoid const krassovsky(_krassovsky_data, Ellipsoid::Identifier::Krassovsky),
    wgs84(_wgs84_data, Ellipsoid::Identifier::WGS84),
    cgcs2000(_cgcs2000_data, Ellipsoid::Identifier::CGCS2000),
    null_ellipsoid(_cgcs2000_data, Ellipsoid::Identifier::EmptyPlaceHolder);

double _ellipsoid_geometry_access_t::a() const {
  return this->m_data.geometry_property.a;
}
double _ellipsoid_geometry_access_t::b() const {
  return this->m_data.geometry_property.b;
}
double _ellipsoid_geometry_access_t::c() const {
  return this->m_data.geometry_property.c;
}
double _ellipsoid_geometry_access_t::alpha() const {
  return this->m_data.geometry_property.alpha;
}
double _ellipsoid_geometry_access_t::e1_2() const {
  return this->m_data.geometry_property.e1_2;
}
double _ellipsoid_geometry_access_t::e2_2() const {
  return this->m_data.geometry_property.e2_2;
}

double _ellipsoid_gravity_access_t::M() const {
  return m_data.gravity_property.M;
}
double _ellipsoid_gravity_access_t::GM() const {
  return m_data.gravity_property.GM;
}
double _ellipsoid_gravity_access_t::p() const {
  return m_data.gravity_property.p;
}
double _ellipsoid_gravity_access_t::omega() const {
  return m_data.gravity_property.omega;
}

Geodetic_coordinate Ellipsoid::coordinate(double lat, double lon) const {
  return Geodetic_coordinate(lat, lon, *this);
}

Geodetic_coordinate Ellipsoid::coordinate(Latitude lat, Longitude lon) const {
  return Geodetic_coordinate(lat, lon, *this);
}

bool Ellipsoid::operator==(Ellipsoid const &other) const {
  return this->m_identifier == other.m_identifier;
}

bool Ellipsoid::operator!=(Ellipsoid const &other) const {
  return !(*this == other);
}

bool Ellipsoid::is_null() const {
  return this->m_identifier == Identifier::EmptyPlaceHolder;
}

Ellipsoid::data_type const &Ellipsoid::data() const { return this->m_data; }

_ellipsoid_geometry_access_t Ellipsoid::geometry() const {
  return _ellipsoid_geometry_access_t(this->data());
}

_ellipsoid_gravity_access_t Ellipsoid::gravity() const {
  return _ellipsoid_gravity_access_t(this->data());
}

std::tuple<double, double, double>
Ellipsoid::principle_curvature_radius(double lat) const {
  GAA_latitude_assert(lat);

  auto const &geo = this->data().geometry_property;
  double sinBp2 = std::pow(std::sin(lat), 2), a = geo.a, e1_2 = geo.e1_2,
         k = 1.0 - e1_2 * sinBp2;
  double M = a * (1.0 - e1_2) * std::pow(k, -1.5), N = a * std::pow(k, -0.5);
  return std::make_tuple(M, N, std::sqrt(M * N));
}

double Ellipsoid::meridian_arc_length(double lat) const {
  auto const &coeff = this->data().quarter_arc_coefficient;
  double a0 = coeff.a0, a2 = coeff.a2, a4 = coeff.a4, a6 = coeff.a6,
         a8 = coeff.a8;
  double B = lat, sin2B = std::sin(2 * B), sin4B = std::sin(4 * B),
         sin6B = std::sin(6 * B), sin8B = std::sin(8 * B);
  double X =
      a0 * rad2deg(B) - a2 * sin2B + a4 * sin4B - a6 * sin6B + a8 * sin8B;
  return X;
}

double Ellipsoid::meridian_arc_bottom_latitude(double len) const {
  auto const &coeff = this->data().quarter_arc_bottom_coefficient;
  auto const &geometry = this->data().geometry_property;
  double B0 = len / (geometry.a * (1 - geometry.e1_2) * coeff.a0),
         sin2B0 = std::sin(2 * B0), sin4B0 = std::sin(4 * B0),
         sin6B0 = std::sin(6 * B0), sin8B0 = std::sin(8 * B0),
         sin10B0 = std::sin(10 * B0), sin12B0 = std::sin(12 * B0),
         sin14B0 = std::sin(14 * B0);

  double rad = B0 + coeff.c1 * sin2B0 + coeff.c2 * sin4B0 + coeff.c3 * sin6B0 +
               coeff.c4 * sin8B0 + coeff.c5 * sin10B0 + coeff.c6 * sin12B0 +
               coeff.c7 * sin14B0;
  return rad;
}

Latitude_auxiliary_constants Ellipsoid::lat_aux(Latitude lat) const {
  return Latitude_auxiliary_constants(lat, *this);
}

Latitude_auxiliary_constants::Latitude_auxiliary_constants(
    Latitude latitude, Ellipsoid const &ellipsoid) {
  auto const &geometry = ellipsoid.geometry();
  t = std::tan(latitude.value());
  nu_2 = geometry.e2_2() * std::pow(std::cos(latitude.value()), 2);
  w = std::sqrt(1 - geometry.e1_2() * std::pow(std::sin(latitude.value()), 2));
  v = std::sqrt(1 + nu_2);
}
} // namespace gaa