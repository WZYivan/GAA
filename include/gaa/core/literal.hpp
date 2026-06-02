#pragma once

#include <string>

#include <gaa/core/pp.hpp>
#include <gaa/wrap/boost/units.hpp>
#include <gaa/wrap/std/any.hpp>

namespace gaa {
enum class Literal_Type {
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

extern Literal_Type literal_detect(std::string const &cnt);
extern Any literal_cast(std::string const &cnt);
extern Any literal_cast(std::string const &cnt, Literal_Type type);
extern std::string enum2str(Literal_Type lt);

extern std::string _literal_cast_string(std::string const &cnt);
extern double _literal_cast_double(std::string const &cnt);
extern int _literal_cast_int(std::string const &cnt);
extern bool _literal_cast_bool(std::string const &cnt);
extern Latitude _literal_cast_Latitude(std::string const &cnt);
extern Longitude _literal_cast_Longitude(std::string const &cnt);
extern Radian _literal_cast_Radian(std::string const &cnt);
extern Arcdeg _literal_cast_Arcdeg(std::string const &cnt);
extern Arcmin _literal_cast_Arcmin(std::string const &cnt);
extern Arcsec _literal_cast_Arcsec(std::string const &cnt);

template <class T> T literal_cast(std::string const &) {}
template <>
inline std::string literal_cast<std::string>(std::string const &cnt) {
  return _literal_cast_string(cnt);
}
template <> inline double literal_cast<double>(std::string const &cnt) {
  return _literal_cast_double(cnt);
}
template <> inline int literal_cast<int>(std::string const &cnt) {
  return _literal_cast_int(cnt);
}
template <> inline bool literal_cast<bool>(std::string const &cnt) {
  return _literal_cast_bool(cnt);
}
template <> inline Latitude literal_cast<Latitude>(std::string const &cnt) {
  return _literal_cast_Latitude(cnt);
}
template <> inline Longitude literal_cast<Longitude>(std::string const &cnt) {
  return _literal_cast_Longitude(cnt);
}
template <> inline Radian literal_cast<Radian>(std::string const &cnt) {
  return _literal_cast_Radian(cnt);
}
template <> inline Arcdeg literal_cast<Arcdeg>(std::string const &cnt) {
  return _literal_cast_Arcdeg(cnt);
}
template <> inline Arcmin literal_cast<Arcmin>(std::string const &cnt) {
  return _literal_cast_Arcmin(cnt);
}
template <> inline Arcsec literal_cast<Arcsec>(std::string const &cnt) {
  return _literal_cast_Arcsec(cnt);
}

template <Literal_Type lt> struct _literal_type {};
template <class T> struct _literal_enum {};
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
#undef GAA_literal
template <Literal_Type lt>
using Literal_type = typename _literal_type<lt>::type;
template <class T>
constexpr static inline Literal_Type literal_enum_v = _literal_enum<T>::value;
} // namespace gaa