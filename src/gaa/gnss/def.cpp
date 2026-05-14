#include <gaa/core/keywords.hpp>
#include <gaa/gnss/def.hpp>

namespace gaa {
std::string enum2str(Satellite_System v) {
  switch (v) {
  case BDS: {
    return "BDS";
  }
  case GPS: {
    return "GPS";
  }
  case GLONASS: {
    return "GLONASS";
  }
  case Satellite_System_COUNT: {
    return "Satellite_System_COUNT";
  }
  case Satellite_System_UNKNOWN: {
    return "Satellite_System_UNKNOWN";
  }
  default:
    gaa_assert(false, "unreachable default case");
  }
}
std::string enum2str(Rinex_type v) {
  switch (v) {
  case Rinex_Navigation: {
    return "Rinex_Navigation";
  }
  case Rinex_type_COUNT: {
    return "Rinex_type_COUNT";
  }
  case Rinex_type_UNKNOWN: {
    return "Rinex_type_UNKNOWN";
  }
  default:
    gaa_assert(false, "unreachable default case");
  }
}
std::string enum2str(UTC_Identifier v) {
  switch (v) {
  case UTC_NIST: {
    return "UTC_NIST";
  }
  case UTC_USNO: {
    return "UTC_USNO";
  }
  case UTC_SU: {
    return "UTC_SU";
  }
  case UTC_BIPM: {
    return "UTC_BIPM";
  }
  case UTC_EU_Lab: {
    return "UTC_EU_Lab";
  }
  case UTC_CRL: {
    return "UTC_CRL";
  }
  case UTC_NTSC_BDS: {
    return "UTC_NTSC_BDS";
  }
  case UTC_Identifier_COUNT: {
    return "UTC_Identifier_COUNT";
  }
  case UTC_Identifier_UNKOWN: {
    return "UTC_Identifier_UNKOWN";
  }
  default:
    gaa_assert(false, "unreachable default case");
  }
}
} // namespace gaa