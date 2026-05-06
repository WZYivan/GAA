#pragma once

#include <gaa/core/config.hpp>

#if !(GAA_MSVC) && (0)

#include <chrono>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <gaa/core/signature_of_deprecated_header.hpp>
#include <gaa/geodesy/ellipsoids.hpp>

namespace gaa::rinex {
std::string label_of(std::string const &s);
std::string content_of(std::string const &s);

enum FileType { Navigation };

enum SatelliteSystem { GPS, GLONASS, Galileo, QZSS, BDS, SBAS_Payload, Mixed };

enum TimeZone { UTC, LCL };

enum IonosphericCorrectionType { GAL, GPSA, GPSB, QZSA, QZSB, BDSA, BDSB };

enum LeapSecondsType { tLS, tLSF, WN, DN };

enum TimeCorrectionType {
  GAUT,
  GPUT,
  SBUT,
  GLUT,
  GPGA,
  GLGP,
  QZGP,
  QZUT,
  BDUT
};

enum UTCIdentifier {
  Unknown = 0,
  UTC_NIST,
  UTC_USNO,
  UTC_SU,
  UTC_BIPM,
  UTC_Europe_Lab,
  UTC_CRL,
  UTC_NSTC_BDS,
  Not_Assigned
};

enum AugmentationSystem { EGNOS, WAAS, MASA, Not_Given };

FileType file_type(char const &c);
SatelliteSystem satellite_system(char const &c);
TimeZone time_zone(std::string const &s);
IonosphericCorrectionType ionospheric_correction_type(std::string const &);
TimeCorrectionType time_correction_type(std::string const &s);
UTCIdentifier utc_identifier(int x);
AugmentationSystem augmentation_system(std::string const &s);

Ellipsoid const &spatial_ref_sys(SatelliteSystem sys);

namespace v302 {
struct Nav_hdr_section {
  // RINEX VERSION / TYPE
  double version;
  FileType file_type;
  SatelliteSystem satellite_system;

  // PGM / RUN BY / DATE
  std::string pgm;
  std::string run_by;
  std::chrono::seconds date;
  TimeZone time_zone;

  // COMMENT
  std::vector<std::string> comments;

  // IONOSPHERIC CORR
  std::map<IonosphericCorrectionType, std::vector<double>> ionospheric_corr;

  // LEAP SECONDS
  std::map<LeapSecondsType, int> leap_seconds;

  // TIME SYSTEM CORR
  TimeCorrectionType time_corr_type;
  double a0, a1;
  int T, W;
  AugmentationSystem S;
  UTCIdentifier utc;

  // END OF HEADER
};

Nav_hdr_section parse_nav_hdr(std::istream &in);

struct Nav_ephemeris_bds {
  // SV /EPOCH / SV CLK
  SatelliteSystem satellite_system;
  int PRN;
  std::chrono::year year;
  std::chrono::month month;
  std::chrono::day day;
  std::chrono::hours hour;
  std::chrono::minutes minute;
  std::chrono::seconds second;

  double clock_bias, clock_drift, clock_drift_rate;

  // BROADCAST ORBIT-1
  double IODE, Crs, Delta_n, M0;

  // BROADCAST ORBIT-2
  double Cuc, e_Eccentricity, Cus, sqrtA;

  // BROADCAST ORBIT-3
  double Toe, Cic, OMEGA0, negCis, Cis;

  // BROADCAST ORBIT-4
  double i0, Crc, omega, OMEGA_DOT;

  // BROADCAST ORBIT-5
  double IDOT, /*... , */ BDT_Week /*, ... */;

  // BROADCAST ORBIT-6
  double SV_accuracy, SatH1, TGD1, TGD2;

  // BROADCAST ORBIT-7
  double transmission_time, IODC /*, ... */ /*, ...*/;
};

using Nav_dat_section_bds = std::vector<Nav_ephemeris_bds>;
Nav_dat_section_bds parse_nav_dat_bds(std::istream &in,
                                      Nav_hdr_section const &hdr);

struct Nav_ephemeris_gps {};

using Nav_data_section_gps = std::vector<Nav_ephemeris_gps>;
Nav_data_section_gps parse_nav_dat_gps(std::istream &in,
                                       Nav_hdr_section const &hdr);
} // namespace v302
} // namespace gaa::rinex

#define GAA_RINEX_char_f(TARGET, FUNC)                                         \
  [&](auto const &ctx) {                                                       \
    char chr = x3::_attr(ctx);                                                 \
    TARGET = FUNC(chr);                                                        \
  }

#define GAA_RINEX_str(TARGET)                                                  \
  [&](auto const &ctx) {                                                       \
    std::string str = x3::_attr(ctx);                                          \
    boost::trim(str);                                                          \
    TARGET = str;                                                              \
  }

#define GAA_RINEX_str_f(TARGET, FUNC)                                          \
  [&](auto const &ctx) {                                                       \
    std::string str = x3::_attr(ctx);                                          \
    boost::trim(str);                                                          \
    TARGET = FUNC(str);                                                        \
  }

#define GAA_RINEX_int(TARGET)                                                  \
  [&](auto const &ctx) {                                                       \
    int val;                                                                   \
    std::string str = x3::_attr(ctx);                                          \
    boost::trim(str);                                                          \
    auto [ptr, ec] =                                                           \
        std::from_chars(str.data(), str.data() + str.length(), val);           \
    gaa_assert(ec == std::errc{},                                              \
               std::format("error parsing `{}` to int", str));                 \
    TARGET = val;                                                              \
  }

#define GAA_RINEX_int_f(TARGET, FUNC)                                          \
  [&](auto const &ctx) {                                                       \
    int val;                                                                   \
    std::string str = x3::_attr(ctx);                                          \
    boost::trim(str);                                                          \
    auto [ptr, ec] =                                                           \
        std::from_chars(str.data(), str.data() + str.length(), val);           \
    gaa_assert(ec == std::errc{},                                              \
               std::format("error parsing `{}` to int", str));                 \
    TARGET = FUNC(val);                                                        \
  }

#define GAA_RINEX_int_a(ACT)                                                   \
  [&](auto const &ctx) {                                                       \
    int val;                                                                   \
    std::string str = x3::_attr(ctx);                                          \
    boost::trim(str);                                                          \
    auto [ptr, ec] =                                                           \
        std::from_chars(str.data(), str.data() + str.length(), val);           \
    gaa_assert(ec == std::errc{},                                              \
               std::format("error parsing `{}` to int", str));                 \
    ACT(val);                                                                  \
  }

#define GAA_RINEX_double(TARGET)                                               \
  [&](auto const &ctx) {                                                       \
    double val;                                                                \
    std::string str = x3::_attr(ctx);                                          \
    boost::trim(str);                                                          \
    auto pos = str.find('D');                                                  \
    if (pos != std::string::npos) {                                            \
      str.replace(pos, 1, "E");                                                \
    }                                                                          \
    auto [ptr, ec] =                                                           \
        std::from_chars(str.data(), str.data() + str.length(), val);           \
    gaa_assert(ec == std::errc{},                                              \
               std::format("error parsing `{}` to double", str));              \
    TARGET = val;                                                              \
  }

#define GAA_RINEX_double_a(ACT)                                                \
  [&](auto const &ctx) {                                                       \
    double val;                                                                \
    std::string str = x3::_attr(ctx);                                          \
    boost::trim(str);                                                          \
    auto pos = str.find('D');                                                  \
    if (pos != std::string::npos) {                                            \
      str.replace(pos, 1, "E");                                                \
    }                                                                          \
    auto [ptr, ec] =                                                           \
        std::from_chars(str.data(), str.data() + str.length(), val);           \
    gaa_assert(ec == std::errc{},                                              \
               std::format("error parsing `{}` to double", str));              \
    ACT(val);                                                                  \
  }

#elif (GAA_USE_GNSSTK)

#include <map>
#include <string>
#include <vector>

#include <gaa/core/keywords.hpp>

namespace gaa {

enum Rinex3NavTimeCorrType {
  GPUT = 1,
  GAUT,
  SBUT,
  GLUT,
  GPGA,
  GAGP,
  GLGP,
  QZGP,
  QZUT,
  BDUT,
  BDGP,
  IRUT,
  IRGP,
  Rinex3NavTimeCorrType_COUNT
};

enum Rinex3NavIonoCorrType { GAL = 1, GPSA, GPSB, Rinex3NavIonoCorrType_COUNT };

struct Rinex3_nav_hdr {
  double version;
  std::string type;
  std::string sys;
  std::string program;
  std::string agency;
  std::string date;
  std::vector<std::string> comments;
  std::map<Rinex3NavTimeCorrType, double> time_corr;
  std::map<Rinex3NavIonoCorrType, double> iono_corr;
  long leap_seconds;
  long leap_delta;
  long leap_week;
  long leap_day;
};

template <Rinex3NavTimeCorrType TimeCorr>
inline double get(Rinex3_nav_hdr const &hdr) noexcept {
  gaa_assert(hdr.time_corr.contains(TimeCorr));
  return hdr.time_corr.at(TimeCorr);
}

template <Rinex3NavIonoCorrType IonoCorr>
inline double get(Rinex3_nav_hdr const &hdr) noexcept {
  gaa_assert(hdr.iono_corr.contains(IonoCorr));
  return hdr.iono_corr.at(IonoCorr);
}

enum SatelliteSystem {
  GPS = 1,
  GLONASS,
  Galileo,
  QZSS,
  BDS,
  SBAS_Payload,
  Mixed,
  SatelliteSystem_COUNT
};

struct Rinex3_nav_dat {
  double days;
  std::string sat_sys;
  short PRN_ID;
  SatelliteSystem sat;
  long xmit_time;
  short week_num;

  double accuracy;
  short health;

  short codeflgs;
  short L2Pdata;
  double IODC;
  double IODE;

  double TauN;
  double GammaN;
  double MFTraw;
  long MFtime;
  short freq_num;
  double age_of_info;

  short data_sources;
  double IOD_nav;

  double acc_code;
  double IODN;

  double Toc;
  double af0;
  double af1;
  double af2;
  double Tgd;
  double Tgd2;

  double Cuc;
  double Cus;
  double Crc;
  double Crs;
  double Cic;
  double Cis;

  double Toe;
  double M0;
  double dn;
  double ecc;
  double Ahalf;
  double OMEGA0;
  double i0;
  double w;
  double OMEGAdot;
  double idot;
  double fit_itv;

  double px;
  double py;
  double pz;
  double vx;
  double vy;
  double vz;
  double ax;
  double ay;
  double az;
};

extern std::pair<Rinex3_nav_hdr, std::vector<Rinex3_nav_dat>>
read_rinex3(std::string const &fname);

extern std::string str(Rinex3NavTimeCorrType v);
extern std::string str(Rinex3NavIonoCorrType v);
extern std::string str(SatelliteSystem v);
extern std::string str(Rinex3_nav_hdr const &hdr);
extern std::string str(Rinex3_nav_dat const &dat);

} // namespace gaa

#endif