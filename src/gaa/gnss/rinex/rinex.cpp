#include <gaa/core/config.hpp>

#if !(GAA_MSVC) && (0)

#include <gaa/core/keywords.hpp>
#include <gaa/gnss/rinex.hpp>

#define STR2ENUM(ENUM)                                                         \
  if (s.compare(#ENUM) == 0) {                                                 \
    return ENUM;                                                               \
  }

namespace gaa::rinex {
std::string label_of(std::string const &s) {
  auto label = s.substr(60, s.length() - 61);
  return label;
}

std::string content_of(std::string const &s) { return s.substr(0, 60); }

FileType file_type(char const &c) {
  switch (c) {
  case 'N':
    return FileType::Navigation;

  default:
    gaa_assert(false, "unreachable default case");
  }
}

SatelliteSystem satellite_system(char const &c) {
  switch (c) {
  case 'G':
    return SatelliteSystem::GPS;
  case 'R':
    return SatelliteSystem::GLONASS;
  case 'E':
    return SatelliteSystem::Galileo;
  case 'J':
    return SatelliteSystem::QZSS;
  case 'C':
    return SatelliteSystem::BDS;
  case 'S':
    return SatelliteSystem::SBAS_Payload;
  case 'M':
    return SatelliteSystem::Mixed;

  default:
    gaa_assert(false, "unreachable default case");
  }
}

TimeZone time_zone(std::string const &s) {
  STR2ENUM(UTC)
  STR2ENUM(LCL) { gaa_assert(false, std::format("unknown time zone `{}`", s)); }
}

IonosphericCorrectionType ionospheric_correction_type(std::string const &s) {
  STR2ENUM(GAL)
  STR2ENUM(GPSA)
  STR2ENUM(GPSB)
  STR2ENUM(QZSA)
  STR2ENUM(QZSB)
  STR2ENUM(BDSA)
  STR2ENUM(BDSB) {
    gaa_assert(false,
               std::format("unknown ionospheric correction type `{}`", s));
  }
}

TimeCorrectionType time_correction_type(std::string const &s) {
  STR2ENUM(GAUT)
  STR2ENUM(GPUT)
  STR2ENUM(SBUT)
  STR2ENUM(GLUT)
  STR2ENUM(GPGA)
  STR2ENUM(GLGP)
  STR2ENUM(QZGP)
  STR2ENUM(QZUT)
  STR2ENUM(BDUT) {
    gaa_assert(false, std::format("unknown time correction type `{}`", s));
  }
}

UTCIdentifier utc_identifier(int x) {
  switch (x) {
  case 0:
    return Unknown;
  case 1:
    return UTC_NIST;
  case 2:
    return UTC_USNO;
  case 3:
    return UTC_SU;
  case 4:
    return UTC_BIPM;
  case 5:
    return UTC_Europe_Lab;
  case 6:
    return UTC_CRL;
  case 7:
    return UTC_NSTC_BDS;
  default:
    return Not_Assigned;
  }
}
AugmentationSystem augmentation_system(std::string const &s) {
  STR2ENUM(EGNOS)
  STR2ENUM(WAAS)
  STR2ENUM(MASA) {
    gaa_assert(false, std::format("unknown augmentation system `{}`", s));
  }
}

Ellipsoid const &spatial_ref_sys(SatelliteSystem sys) {
  switch (sys) {
  case BDS:
    return cgcs2000;

  default:
    gaa_assert(false, "unreachable default case");
  }
}
} // namespace gaa::rinex

#elif (GAA_USE_GNSSTK)

#include <filesystem>
#include <print>

#include <gaa/core/signature_of_imcoplete_header.hpp>
#include <gaa/gnss/rinex.hpp>

#include <gnsstk/Rinex3NavData.hpp>
#include <gnsstk/Rinex3NavHeader.hpp>
#include <gnsstk/Rinex3NavStream.hpp>

namespace fs = std::filesystem;
namespace gtk = gnsstk;

namespace gaa {

static bool
_convert_Rinex3NavHeader_to_Rinex3_nav_hdr(gtk::Rinex3NavHeader &src,
                                           Rinex3_nav_hdr &dest) {

  dest.agency.swap(src.fileAgency);
  dest.comments.swap(src.commentList);
  dest.date.swap(src.date);
  dest.program.swap(dest.program);
  dest.sys.swap(dest.sys);
  dest.type.swap(dest.sys);
  dest.leap_day = src.leapDay;
  dest.leap_delta = src.leapDelta;
  dest.leap_seconds = src.leapSeconds;
  dest.leap_week = src.leapWeek;
  dest.version = src.version;
  dest.type.swap(src.fileType);

  for (auto const &[k, v] : src.mapTimeCorr) {
    std::println("time corr: [{}] => {}", k, v.asString());
  }

  for (auto const &[k, v] : src.mapIonoCorr) {
    std::println("iono corr: [{}] => {}", k, v.asString());
  }

  return true;
}

static bool _convert_Rinex3NavData_to_Rinex3_nav_dat(gtk::Rinex3NavData &src,
                                                     Rinex3_nav_dat &dest) {
  dest.days = src.time.getDays();
  dest.sat_sys.swap(src.satSys);
  dest.PRN_ID = src.PRNID;
  switch (src.sat.system) {
  case gtk::SatelliteSystem::GPS: {
    dest.sat = GPS;
    break;
  }
  case gtk::SatelliteSystem::Glonass: {
    dest.sat = GLONASS;
    break;
  }
  case gtk::SatelliteSystem::Galileo: {
    dest.sat = Galileo;
    break;
  }
  case gtk::SatelliteSystem::QZSS: {
    dest.sat = QZSS;
    break;
  }
  case gtk::SatelliteSystem::BeiDou: {
    dest.sat = BDS;
    break;
  }
  default:
    gaa_assert(false, "unreachable default case");
  }
  dest.xmit_time = src.xmitTime;
  dest.week_num = src.weeknum;
  dest.accuracy = src.accuracy;
  dest.health = src.health;
  dest.codeflgs = src.codeflgs;
  dest.L2Pdata = src.L2Pdata;
  dest.IODC = src.IODC;
  dest.IODE = src.IODE;
  dest.TauN = src.TauN;
  dest.GammaN = src.GammaN;
  dest.MFTraw = src.MFTraw;
  dest.MFtime = src.MFtime;
  dest.freq_num = src.freqNum;
  dest.age_of_info = src.ageOfInfo;
  dest.data_sources = src.datasources;
  dest.IOD_nav = src.IODnav;
  dest.acc_code = src.accCode;
  dest.IODN = src.IODN;
  dest.Toc = src.Toc;
  dest.af0 = src.af0;
  dest.af1 = src.af1;
  dest.af2 = src.af2;
  dest.Tgd = src.Tgd;
  dest.Tgd2 = src.Tgd2;
  dest.Cuc = src.Cuc;
  dest.Cus = src.Cus;
  dest.Crc = src.Crc;
  dest.Crs = src.Crs;
  dest.Cic = src.Cic;
  dest.Cis = src.Cis;
  dest.Toe = src.Toe;
  dest.M0 = src.M0;
  dest.dn = src.dn;
  dest.ecc = src.ecc;
  dest.Ahalf = src.Ahalf;
  dest.OMEGA0 = src.OMEGA0;
  dest.i0 = src.i0;
  dest.w = src.w;
  dest.OMEGAdot = src.OMEGAdot;
  dest.idot = src.idot;
  dest.fit_itv = src.fitint;
  dest.px = src.px;
  dest.py = src.py;
  dest.pz = src.pz;
  dest.vx = src.vx;
  dest.vy = src.vy;
  dest.vz = src.vz;
  dest.ax = src.ax;
  dest.ay = src.ay;
  dest.az = src.az;
  return true;
}

std::pair<Rinex3_nav_hdr, std::vector<Rinex3_nav_dat>>
read_rinex3(std::string const &fname) {
  fs::path fpath{fname};
  gaa_assert(fs::exists(fpath));

  gtk::Rinex3NavStream rin{fpath.c_str(), std::ios::in};
  gaa_assert(rin.is_open());

  std::pair<Rinex3_nav_hdr, std::vector<Rinex3_nav_dat>> hdr_dat_list;
  Rinex3_nav_hdr &hdr = hdr_dat_list.first;
  std::vector<Rinex3_nav_dat> dat_list = hdr_dat_list.second;
  Rinex3_nav_dat dat;

  gtk::Rinex3NavHeader gtk_hdr;
  rin >> gtk_hdr;
  gaa_assert(_convert_Rinex3NavHeader_to_Rinex3_nav_hdr(gtk_hdr, hdr));

  gtk::Rinex3NavData gtk_dat;
  while (rin.good()) {
    rin >> gtk_dat;
    gaa_assert(_convert_Rinex3NavData_to_Rinex3_nav_dat(gtk_dat, dat));
    dat_list.emplace_back(dat);
  }

  return hdr_dat_list;
}

std::string str(Rinex3NavTimeCorrType v) {
  switch (v) {
  case GPUT:
    return "GPUT";
  case GAUT:
    return "GAUT";
  case SBUT:
    return "SBUT";
  case GLUT:
    return "GLUT";
  case GPGA:
    return "GPGA";
  case GAGP:
    return "GAGP";
  case GLGP:
    return "GLGP";
  case QZGP:
    return "QZGP";
  case QZUT:
    return "QZUT";
  case BDUT:
    return "BDUT";
  case BDGP:
    return "BDGP";
  case IRUT:
    return "IRUT";
  case IRGP:
    return "IRGP";
  default:
    return "UNKNOWN";
  }
}

std::string str(Rinex3NavIonoCorrType v) {
  switch (v) {
  case GAL:
    return "GAL";
  case GPSA:
    return "GPSA";
  case GPSB:
    return "GPSB";
  default:
    return "UNKNOWN";
  }
}

std::string str(SatelliteSystem v) {
  switch (v) {
  case GPS:
    return "GPS";
  case GLONASS:
    return "GLONASS";
  case Galileo:
    return "Galileo";
  case QZSS:
    return "QZSS";
  case BDS:
    return "BDS";
  case SBAS_Payload:
    return "SBAS_Payload";
  case Mixed:
    return "Mixed";
  default:
    return "UNKNOWN";
  }
}

static inline std::string _json_escape(const std::string &s) {
  std::string out;
  for (char c : s) {
    if (c == '"')
      out += "\\\"";
    else if (c == '\\')
      out += "\\\\";
    else
      out += c;
  }
  return out;
}

std::string str(Rinex3_nav_hdr const &hdr) {
  std::ostringstream oss;
  oss << "{";
  oss << "\"version\":" << hdr.version << ",";
  oss << "\"type\":\"" << _json_escape(hdr.type) << "\",";
  oss << "\"sys\":\"" << _json_escape(hdr.sys) << "\",";
  oss << "\"program\":\"" << _json_escape(hdr.program) << "\",";
  oss << "\"agency\":\"" << _json_escape(hdr.agency) << "\",";
  oss << "\"date\":\"" << _json_escape(hdr.date) << "\",";
  oss << "\"comments\":[";
  for (size_t i = 0; i < hdr.comments.size(); ++i) {
    if (i > 0)
      oss << ",";
    oss << "\"" << _json_escape(hdr.comments[i]) << "\"";
  }
  oss << "],";
  oss << "\"time_corr\":{";
  bool first = true;
  for (const auto &kv : hdr.time_corr) {
    if (!first)
      oss << ",";
    oss << "\"" << str(kv.first) << "\":" << kv.second;
    first = false;
  }
  oss << "},";
  oss << "\"iono_corr\":{";
  first = true;
  for (const auto &kv : hdr.iono_corr) {
    if (!first)
      oss << ",";
    oss << "\"" << str(kv.first) << "\":" << kv.second;
    first = false;
  }
  oss << "},";
  oss << "\"leap_seconds\":" << hdr.leap_seconds << ",";
  oss << "\"leap_delta\":" << hdr.leap_delta << ",";
  oss << "\"leap_week\":" << hdr.leap_week << ",";
  oss << "\"leap_day\":" << hdr.leap_day;
  oss << "}";
  return oss.str();
}

std::string str(Rinex3_nav_dat const &dat) {
  std::ostringstream oss;
  oss << "{";
  oss << "\"days\":" << dat.days << ",";
  oss << "\"sat_sys\":\"" << _json_escape(dat.sat_sys) << "\",";
  oss << "\"PRN_ID\":" << dat.PRN_ID << ",";
  oss << "\"sat\":\"" << str(dat.sat) << "\",";
  oss << "\"xmit_time\":" << dat.xmit_time << ",";
  oss << "\"week_num\":" << dat.week_num << ",";
  oss << "\"accuracy\":" << dat.accuracy << ",";
  oss << "\"health\":" << dat.health << ",";
  oss << "\"codeflgs\":" << dat.codeflgs << ",";
  oss << "\"L2Pdata\":" << dat.L2Pdata << ",";
  oss << "\"IODC\":" << dat.IODC << ",";
  oss << "\"IODE\":" << dat.IODE << ",";
  oss << "\"TauN\":" << dat.TauN << ",";
  oss << "\"GammaN\":" << dat.GammaN << ",";
  oss << "\"MFTraw\":" << dat.MFTraw << ",";
  oss << "\"MFtime\":" << dat.MFtime << ",";
  oss << "\"freq_num\":" << dat.freq_num << ",";
  oss << "\"age_of_info\":" << dat.age_of_info << ",";
  oss << "\"data_sources\":" << dat.data_sources << ",";
  oss << "\"IOD_nav\":" << dat.IOD_nav << ",";
  oss << "\"acc_code\":" << dat.acc_code << ",";
  oss << "\"IODN\":" << dat.IODN << ",";
  oss << "\"Toc\":" << dat.Toc << ",";
  oss << "\"af0\":" << dat.af0 << ",";
  oss << "\"af1\":" << dat.af1 << ",";
  oss << "\"af2\":" << dat.af2 << ",";
  oss << "\"Tgd\":" << dat.Tgd << ",";
  oss << "\"Tgd2\":" << dat.Tgd2 << ",";
  oss << "\"Cuc\":" << dat.Cuc << ",";
  oss << "\"Cus\":" << dat.Cus << ",";
  oss << "\"Crc\":" << dat.Crc << ",";
  oss << "\"Crs\":" << dat.Crs << ",";
  oss << "\"Cic\":" << dat.Cic << ",";
  oss << "\"Cis\":" << dat.Cis << ",";
  oss << "\"Toe\":" << dat.Toe << ",";
  oss << "\"M0\":" << dat.M0 << ",";
  oss << "\"dn\":" << dat.dn << ",";
  oss << "\"ecc\":" << dat.ecc << ",";
  oss << "\"Ahalf\":" << dat.Ahalf << ",";
  oss << "\"OMEGA0\":" << dat.OMEGA0 << ",";
  oss << "\"i0\":" << dat.i0 << ",";
  oss << "\"w\":" << dat.w << ",";
  oss << "\"OMEGAdot\":" << dat.OMEGAdot << ",";
  oss << "\"idot\":" << dat.idot << ",";
  oss << "\"fit_itv\":" << dat.fit_itv << ",";
  oss << "\"px\":" << dat.px << ",";
  oss << "\"py\":" << dat.py << ",";
  oss << "\"pz\":" << dat.pz << ",";
  oss << "\"vx\":" << dat.vx << ",";
  oss << "\"vy\":" << dat.vy << ",";
  oss << "\"vz\":" << dat.vz << ",";
  oss << "\"ax\":" << dat.ax << ",";
  oss << "\"ay\":" << dat.ay << ",";
  oss << "\"az\":" << dat.az;
  oss << "}";
  return oss.str();
}

} // namespace gaa

#endif