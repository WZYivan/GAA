#pragma once

#include <string>

#include <gaa/core/literal/def.hpp>
#include <gaa/wrap/std/any.hpp>

namespace gaa {

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
} // namespace gaa