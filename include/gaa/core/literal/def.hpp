#pragma once

#include <gaa/core/pp.hpp>
#include <gaa/wrap/boost/units.hpp>

namespace gaa {
enum class Literal_Type {
  Charater,
  String,
  Digital,
  Integer,
  Logical,
  Latitude,
  Longitude,
  Radian,
  Arcdeg,
  Arcmin,
  Arcsec
};

template <Literal_Type lt> struct _literal_type {
  static_assert(false, "this Literal_Type is not supported");
};
template <class T> struct _literal_enum {
  static_assert(false, "this type is not supported");
};
#define GAA_literal(ENUM, TYPE)                                                \
  template <> struct _literal_type<ENUM> {                                     \
    using type = GAA_PP_STRIP_PARAM TYPE;                                      \
  };                                                                           \
  template <> struct _literal_enum<GAA_PP_STRIP_PARAM TYPE> {                  \
    constexpr static Literal_Type value = ENUM;                                \
  }
GAA_literal(Literal_Type::Arcdeg, (Arcdeg));
GAA_literal(Literal_Type::Arcmin, (Arcmin));
GAA_literal(Literal_Type::Arcsec, (Arcsec));
GAA_literal(Literal_Type::Digital, (double));
GAA_literal(Literal_Type::Integer, (int));
GAA_literal(Literal_Type::Latitude, (Latitude));
GAA_literal(Literal_Type::Logical, (bool));
GAA_literal(Literal_Type::Longitude, (Longitude));
GAA_literal(Literal_Type::Radian, (Radian));
GAA_literal(Literal_Type::String, (std::string));
GAA_literal(Literal_Type::Charater, (char));
#undef GAA_literal
template <Literal_Type lt>
using Literal_type = typename _literal_type<lt>::type;
template <class T>
constexpr static inline Literal_Type literal_enum_v = _literal_enum<T>::value;
} // namespace gaa