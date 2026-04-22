#pragma once

#include <boost/units/quantity.hpp>
#include <boost/units/systems/angle/degrees.hpp>
#include <boost/units/systems/si/plane_angle.hpp>
#include <boost/units/is_unit.hpp>
#include <boost/units/is_quantity.hpp>

namespace gaa
{

    template <class Unit>
    struct _quantity_traits
    {
        using unit_type = Unit;
        constexpr static unit_type unit{};
        using base_type = boost::units::quantity<unit_type>;
    };

    template <class Unit>
        requires boost::units::is_unit<Unit>::value
    class _basic_quantity_t
        : public _quantity_traits<Unit>,
          public boost::units::quantity<Unit>
    {
    public:
        using Traits = _quantity_traits<Unit>;
        using Traits::base_type;
        using Traits::unit;
        using Traits::unit_type;
        using quantity = boost::units::quantity<Unit>;

    public:
        virtual ~_basic_quantity_t() = default;
        explicit _basic_quantity_t(double val) : quantity(val * unit) {};

        using quantity::quantity;
    };

    template <class>
    struct _is_basic_quantity_t
        : public boost::mpl::false_
    {
    };

    template <class U>
    struct _is_basic_quantity_t<_basic_quantity_t<U>>
        : public boost::mpl::true_
    {
    };

    template <class Q>
        requires requires {
            typename Q::unit_type;
        } && boost::units::is_unit<typename Q::unit_type>::value
    struct Unit_of
    {
        using unit_type = typename Q::unit_type;
        constexpr static unit_type unit{};
    };

    template <class Q>
    constexpr inline typename Unit_of<Q>::unit_type unit_of = Unit_of<Q>::unit;

    template <class Q>
    concept Is_Quantity =
        boost::units::is_quantity<Q>::value ||
        _is_basic_quantity_t<Q>::value;

    template <class Q1, class Q2>
    concept Compatible_Quantity =
        (Is_Quantity<Q1> &&
         Is_Quantity<Q2>) &&
        (std::same_as<Q1, Q2> ||
         std::constructible_from<Q1, Q2>);

    using Radian = _basic_quantity_t<boost::units::si::plane_angle>;
    using Arcdeg = _basic_quantity_t<boost::units::degree::plane_angle>;

    using _arcmin_base_unit = boost::units::scaled_base_unit<
        boost::units::angle::degree_base_unit,
        boost::units::scale<60, boost::units::static_rational<-1>>>;
    using _arcmin_unit = boost::units::unit<
        boost::units::plane_angle_dimension,
        boost::units::make_system<_arcmin_base_unit>::type>;

    using Arcmin = _basic_quantity_t<_arcmin_unit>;

    using _arcsec_base_unit = boost::units::scaled_base_unit<
        boost::units::angle::degree_base_unit,
        boost::units::scale<3600, boost::units::static_rational<-1>>>;
    using _arcsec_unit = boost::units::unit<
        boost::units::plane_angle_dimension,
        boost::units::make_system<_arcsec_base_unit>::type>;

    using Arcsec = _basic_quantity_t<_arcsec_unit>;

    class Latitude
        : public _basic_quantity_t<boost::units::si::plane_angle>
    {
    public:
        using base_type = _basic_quantity_t<boost::units::si::plane_angle>;
        using base_type::unit;
        using base_type::unit_type;

    private:
        bool validate()
        {
            return Arcdeg(std::abs(this->value())) <= 90 * Arcdeg::unit;
        }

    public:
        ~Latitude() = default;
        explicit Latitude(double val) : base_type(val)
        {
            validate();
        }

        template <Compatible_Quantity<base_type> Q>
        Latitude(Q const &q) : base_type(q)
        {
            validate();
        }
    };

    class Longitude
        : public _basic_quantity_t<boost::units::si::plane_angle>
    {
    public:
        using base_type = _basic_quantity_t<boost::units::si::plane_angle>;
        using base_type::unit;
        using base_type::unit_type;

    private:
        bool validate()
        {
            return Arcdeg(std::abs(this->value())) <= 90 * Arcdeg::unit;
        }

    public:
        ~Longitude() = default;
        explicit Longitude(double val) : base_type(val)
        {
            validate();
        }

        template <Compatible_Quantity<base_type> Q>
        Longitude(Q const &q) : base_type(q)
        {
            validate();
        }
    };

#define GAA_UNITS_wrap_std_trigonometric_func(FUNC)            \
    template <Compatible_Quantity<Radian> Q>                   \
    double FUNC(Q const &q)                                    \
    {                                                          \
        using T = std::remove_cvref_t<Q>;                      \
        if constexpr (std::same_as<T, Radian>)                 \
        {                                                      \
            return std::FUNC(q.value());                       \
        }                                                      \
        else if constexpr (std::constructible_from<Radian, T>) \
        {                                                      \
            return std::FUNC(Radian(q).value());               \
        }                                                      \
    }

    GAA_UNITS_wrap_std_trigonometric_func(sin);
    GAA_UNITS_wrap_std_trigonometric_func(cos);
    GAA_UNITS_wrap_std_trigonometric_func(tan);

    // #undef GAA_UNITS_wrap_std_trigonometric_func

#define GAA_UNITS_OP(X, OP, Y)                                \
    []<class Xt, class Yt>(Xt const &x, Yt const &y)          \
        requires gaa::Is_Quantity<Xt> || gaa::Is_Quantity<Xt> \
    {                                                         \
        if constexpr (gaa::Is_Quantity<Xt>)                   \
        {                                                     \
            return x OP(y * x.unit);                          \
        }                                                     \
        else if constexpr (gaa::Is_Quantity<Yt>)              \
        {                                                     \
            return y OP(x * y.unit);                          \
        }                                                     \
    }(X, Y)
}