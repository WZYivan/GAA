/*
wrapper of Boost.Units for some specialized unit:
> Radian
> Azimuth
> Latitude
> Longitude
> Arcdeg
> Arcmin
> Arcsec
> Angle (not impl)
*/

#pragma once

#include <concepts>
#include <format>
#include <type_traits>

#include <boost/units/is_quantity.hpp>
#include <boost/units/is_unit.hpp>
#include <boost/units/quantity.hpp>
#include <boost/units/systems/angle/degrees.hpp>
#include <boost/units/systems/si/plane_angle.hpp>

#include <gaa/core/buildsystem/config.hpp>
#include <gaa/core/keywords.hpp>
#include <gaa/core/math.hpp>

#ifdef GAA_UNIT_NEVER_VALIDATE
#define GAA_UNIT_validate_switch 1
#else
#define GAA_UNIT_validate_switch 0
#endif

namespace gaa {
namespace units {
/// traits for mpl
template <class Unit> struct _quantity_traits {
  using unit_type = Unit;
  constexpr static unit_type unit{};
  using base_type = boost::units::quantity<unit_type>;
};

/*
this base type is basically for mpl of derived type
*/
template <class Unit>
  requires boost::units::is_unit<Unit>::value
class _basic_quantity_t : public _quantity_traits<Unit>,
                          public boost::units::quantity<Unit> {
public:
  using Traits = _quantity_traits<Unit>;
  using Traits::unit;
  using typename Traits::base_type;
  using typename Traits::unit_type;
  using quantity = boost::units::quantity<Unit>;

public:
  virtual ~_basic_quantity_t() = default;
  explicit _basic_quantity_t(double val) : quantity(val * unit) {};

  using quantity::quantity;
};

template <class> struct _is_basic_quantity_t : public boost::mpl::false_ {};

template <class U>
struct _is_basic_quantity_t<_basic_quantity_t<U>> : public boost::mpl::true_ {};

template <class U>
concept Is_Unit = boost::units::is_unit<U>::value;

// template <class Q>
//     requires requires {
//         typename Q::unit_type;
//     } && Is_Unit<typename Q::unit_type>
// struct Unit_of
// {
//     using unit_type = typename Q::unit_type;
//     constexpr static unit_type unit{};
// };

// template <Is_Unit U>
// struct Unit_of<boost::units::quantity<U>>
// {
//     using unit_type = U;
//     constexpr static unit_type unit{};
// };

// template <Is_Unit U>
// struct Unit_of<_basic_quantity_t<U>>
// {
//     using unit_type = U;
//     constexpr static unit_type unit{};
// };

// template <class Q>
// constexpr inline typename Unit_of<Q>::unit_type unit_of = Unit_of<Q>::unit;

template <class Q>
concept Is_Quantity =
    boost::units::is_quantity<Q>::value || _is_basic_quantity_t<Q>::value;

template <class Q1, class Q2>
concept Compatible_Quantity =
    (Is_Quantity<Q1> && Is_Quantity<Q2>) &&
    (std::same_as<Q1, Q2> || std::constructible_from<Q1, Q2>);
/// traits for mpl

using Radian = _basic_quantity_t<boost::units::si::plane_angle>;
using Arcdeg = _basic_quantity_t<boost::units::degree::plane_angle>;
using Azimuth = Radian;

using _arcmin_base_unit = boost::units::scaled_base_unit<
    boost::units::angle::degree_base_unit,
    boost::units::scale<60, boost::units::static_rational<-1>>>;
using _arcmin_unit =
    boost::units::unit<boost::units::plane_angle_dimension,
                       boost::units::make_system<_arcmin_base_unit>::type>;

using Arcmin = _basic_quantity_t<_arcmin_unit>;

using _arcsec_base_unit = boost::units::scaled_base_unit<
    boost::units::angle::degree_base_unit,
    boost::units::scale<3600, boost::units::static_rational<-1>>>;
using _arcsec_unit =
    boost::units::unit<boost::units::plane_angle_dimension,
                       boost::units::make_system<_arcsec_base_unit>::type>;

using Arcsec = _basic_quantity_t<_arcsec_unit>;

class Latitude : public Radian {
public:
  using base_type = Radian;
  using base_type::unit;
  using base_type::unit_type;

private:
  void validate() {
    /// reject illegal value, must in [-90, 90] deg
    gaa_assert((GAA_UNIT_validate_switch) ||
                   std::abs(this->value()) <= std::numbers::pi / 2,
               "invalid value: \'{}\'", this->value());
  }

public:
  ~Latitude() = default;
  Latitude() = default;
  explicit Latitude(double val) : base_type(val) { validate(); }

  template <Compatible_Quantity<base_type> Q>
  Latitude(Q const &q)
      : base_type(
            boost::units::quantity<boost::units::si::plane_angle>(q).value()) {
    validate();
  }
};

class Longitude : public Radian {
public:
  using base_type = Radian;
  using base_type::unit;
  using base_type::unit_type;

private:
  void validate() {
    /// reject illegal value, must in [-180, 180] deg
    gaa_assert((GAA_UNIT_validate_switch) ||
                   std::abs(this->value()) <= std::numbers::pi,
               "invalid value: \'{}\'", this->value());
  }

public:
  ~Longitude() = default;
  Longitude() = default;
  explicit Longitude(double val) : base_type(val) { validate(); }

  template <Compatible_Quantity<base_type> Q>
  Longitude(Q const &q)
      : base_type(
            boost::units::quantity<boost::units::si::plane_angle>(q).value()) {
    validate();
  }
};
} // namespace units

using units::Arcdeg;
using units::Arcmin;
using units::Arcsec;
using units::Azimuth;
using units::Latitude;
using units::Longitude;
using units::Radian;

/// convenient unit convert functions
template <class Q> double rad(Q const &q) {
  if constexpr (std::same_as<Radian, Q>) {
    return q.value();
  } else if constexpr (std::constructible_from<Radian, Q>) {
    return Radian(q).value();
  } else if constexpr (std::is_base_of_v<Radian, Q>) {
    return q.value();
  } else {
    static_assert(false, "can't convert this type to radian");
  }
}

extern double rad(Radian const &r);
extern double rad(Latitude const &lat);
extern double rad(Longitude const &lon);

extern double deg(Radian const &r);
extern double deg(Latitude const &lat);
extern double deg(Longitude const &lon);

extern Radian rad(double r);
extern Latitude lat(double r);
extern Longitude lon(double r);
/// convenient unit convert functions

/// may deprecated
inline Radian operator""_rad(long double r) {
  return rad(static_cast<double>(r));
}

#define GAA_UNITS_OP(X, OP, Y)                                                 \
  []<class Xt, class Yt>(Xt const &x, Yt const &y)                             \
    requires gaa::Is_Quantity<Xt> || gaa::Is_Quantity<Yt>                      \
  {                                                                            \
    if constexpr (gaa::Is_Quantity<Xt>) {                                      \
      return x OP(y * x.unit);                                                 \
    } else if constexpr (gaa::Is_Quantity<Yt>) {                               \
      return y OP(x * y.unit);                                                 \
    }                                                                          \
  }(X, Y)
/// may deprecated
} // namespace gaa