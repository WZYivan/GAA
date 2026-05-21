#pragma once

#include <string>

#include <gaa/geodesy/ellipsoids.hpp>

namespace gaa {
enum Satellite_System {
  BDS = 1,
  GPS,
  GLONASS,
  Satellite_System_COUNT,
  Satellite_System_UNKNOWN
};

enum Rinex_type { Rinex_Navigation = 1, Rinex_type_COUNT, Rinex_type_UNKNOWN };

enum UTC_Identifier {
  UTC_NIST = 1,
  UTC_USNO,
  UTC_SU,
  UTC_BIPM,
  UTC_EU_Lab,
  UTC_CRL,
  UTC_NTSC_BDS,
  UTC_Identifier_COUNT,
  UTC_Identifier_UNKOWN
};

extern std::string enum2str(Satellite_System v);
extern std::string enum2str(Rinex_type v);
extern std::string enum2str(UTC_Identifier v);

extern Ellipsoid_ref ellipsoid_of(Satellite_System v);
} // namespace gaa