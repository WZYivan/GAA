#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

#include <gaa/core/etc.hpp>
#include <gaa/core/literal.hpp>

namespace gaa {

Literal_Type literal_detect(std::string const &cnt) {
  static struct {
    boost::regex digital{R"(^[+-]?[0-9]+\.[0-9]*([EeD][+-]?[0-9]+)?$)"};
    boost::regex integer{R"(^[+-]?[0-9]+$)"};
    boost::regex logical{R"(^True|False|true|false$)"};
    boost::regex latitude{R"(^[+-]?[0-9]+\.[0-9]*([Ee][+-]?[0-9]+)?_lat$)"};
    boost::regex longitude{R"(^[+-]?[0-9]+\.[0-9]*([Ee][+-]?[0-9]+)?_lon$)"};
    boost::regex radian{R"(^[+-]?[0-9]+\.[0-9]*([Ee][+-]?[0-9]+)?_rad$)"};
    boost::regex arcdeg{R"(^[+-]?[0-9]+\.[0-9]*([Ee][+-]?[0-9]+)?_deg$)"};
    boost::regex arcmin{R"(^[+-]?[0-9]+\.[0-9]*([Ee][+-]?[0-9]+)?_min$)"};
    boost::regex arcsec{R"(^[+-]?[0-9]+\.[0-9]*([Ee][+-]?[0-9]+)?_sec$)"};
  } pattern;

  using enum Literal_Type;

  if (boost::regex_match(cnt, pattern.digital)) {
    return Digital;
  } else if (boost::regex_match(cnt, pattern.integer)) {
    return Integer;
  } else if (boost::regex_match(cnt, pattern.logical)) {
    return Logical;
  } else if (boost::regex_match(cnt, pattern.latitude)) {
    return Latitude;
  } else if (boost::regex_match(cnt, pattern.longitude)) {
    return Longitude;
  } else if (boost::regex_match(cnt, pattern.radian)) {
    return Radian;
  } else if (boost::regex_match(cnt, pattern.arcdeg)) {
    return Arcdeg;
  } else if (boost::regex_match(cnt, pattern.arcmin)) {
    return Arcmin;
  } else if (boost::regex_match(cnt, pattern.arcsec)) {
    return Arcsec;
  } else {
    return String;
  }
}

Any literal_cast(std::string const &cnt) {
  return literal_cast(cnt, literal_detect(cnt));
}

Any literal_cast(std::string const &cnt, Literal_Type type) {
  using enum Literal_Type;
  switch (type) {
  case String: {
    return literal_cast<Literal_type<String>>(cnt);
  }
  case Digital: {
    return literal_cast<Literal_type<Digital>>(cnt);
  }
  case Integer: {
    return literal_cast<Literal_type<Integer>>(cnt);
  }
  case Logical: {
    return literal_cast<Literal_type<Logical>>(cnt);
  }
  case Latitude: {
    return literal_cast<Literal_type<Latitude>>(cnt);
  }
  case Longitude: {
    return literal_cast<Literal_type<Longitude>>(cnt);
  }
  case Radian: {
    return literal_cast<Literal_type<Radian>>(cnt);
  }
  case Arcdeg: {
    return literal_cast<Literal_type<Arcdeg>>(cnt);
  }
  case Arcmin: {
    return literal_cast<Literal_type<Arcmin>>(cnt);
  }
  case Arcsec: {
    return literal_cast<Literal_type<Arcsec>>(cnt);
  }
  default: {
    gaa_fail("unreachable default case");
  }
  }
}

std::string enum2str(Literal_Type lt) {
  using enum Literal_Type;
  switch (lt) {
  case String: {
    return "String";
  }
  case Digital: {
    return "Digital";
  }
  case Integer: {
    return "Integer";
  }
  case Logical: {
    return "Logical";
  }
  case Latitude: {
    return "Latitude";
  }
  case Longitude: {
    return "Longitude";
  }
  case Radian: {
    return "Radian";
  }
  case Arcdeg: {
    return "Arcdeg";
  }
  case Arcmin: {
    return "Arcmin";
  }
  case Arcsec: {
    return "Arcsec";
  }
  default: {
    gaa_fail("unreachable default case");
  }
  }
}

std::string _literal_cast_string(std::string const &cnt) { return cnt; }
double _literal_cast_double(std::string const &cnt) {
  double val;
  if (cnt.contains('D')) {
    std::string replaced = boost::replace_first_copy(cnt, "D", "E");
    gaa_assert(from_str(replaced, val), "literal cast to double fail: {}",
               replaced);
  } else {
    gaa_assert(from_str(cnt, val), "literal cast to double fail: {}", cnt);
  }
  return val;
}
int _literal_cast_int(std::string const &cnt) {
  int val;
  gaa_assert(from_str(cnt, val), "literal cast to int fail: {}", cnt);
  return val;
}
bool _literal_cast_bool(std::string const &cnt) {
  if (cnt == "True" || cnt == "true") {
    return true;
  } else if (cnt == "False" || cnt == "false") {
    return false;
  } else {
    gaa_fail("literal cast to bool fail: {}", cnt);
  }
}
Latitude _literal_cast_Latitude(std::string const &cnt) {
  double val;
  gaa_assert(from_str(cnt.substr(0, cnt.size() - 4), val),
             "literal cast to latitude fail: {}", cnt);
  return lat(val);
}
Longitude _literal_cast_Longitude(std::string const &cnt) {
  double val;
  gaa_assert(from_str(cnt.substr(0, cnt.size() - 4), val),
             "literal cast to longitude fail: {}", cnt);
  return lon(val);
}
Radian _literal_cast_Radian(std::string const &cnt) {
  double val;
  gaa_assert(from_str(cnt.substr(0, cnt.size() - 4), val),
             "literal cast to radian fail: {}", cnt);
  return rad(val);
}
Arcdeg _literal_cast_Arcdeg(std::string const &cnt) {
  double val;
  gaa_assert(from_str(cnt.substr(0, cnt.size() - 4), val),
             "literal cast to arcdeg fail: {}", cnt);
  return Arcdeg(val);
}
Arcmin _literal_cast_Arcmin(std::string const &cnt) {
  double val;
  gaa_assert(from_str(cnt.substr(0, cnt.size() - 4), val),
             "literal cast to arcmin fail: {}", cnt);
  return Arcmin(val);
}
Arcsec _literal_cast_Arcsec(std::string const &cnt) {
  double val;
  gaa_assert(from_str(cnt.substr(0, cnt.size() - 4), val),
             "literal cast to arcsec fail: {}", cnt);
  return Arcsec(val);
}
} // namespace gaa