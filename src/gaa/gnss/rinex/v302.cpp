#include <gaa/core/config.hpp>

#if !(GAA_MSVC) && (0)

#include <charconv>

#include <boost/algorithm/string.hpp>
#include <boost/spirit/home/x3.hpp>

#include <gaa/core/keywords.hpp>
#include <gaa/gnss/rinex.hpp>

#define Af GAA_RINEX_char_f
#define STR GAA_RINEX_str
#define STRf GAA_RINEX_str_f
#define I GAA_RINEX_int
#define If GAA_RINEX_int_f
#define Ia GAA_RINEX_int_a
#define D GAA_RINEX_double
#define Da GAA_RINEX_double_a

namespace gaa::rinex::v302 {
namespace x3 = boost::spirit::x3;

Nav_hdr_section parse_nav_hdr(std::istream &in) {
  gaa_assert(in.good(), "input stream not good");

  std::string line;
  Nav_hdr_section hdr;
  bool end_normally = false;

  int tcl_counter = 0;

  while (std::getline(in, line)) {
    if (line.empty()) {
      continue;
    }

    auto label = label_of(line);

    if (label.compare("RINEX VERSION / TYPE") == 0) {
      std::string cnt = content_of(line);

      auto r = x3::repeat(9)[x3::char_][(D(hdr.version))] >>
               x3::repeat(11)[x3::char_] >>
               x3::char_[(Af(hdr.file_type, file_type))] >>
               x3::repeat(19)[x3::char_] >>
               x3::char_[(Af(hdr.satellite_system, satellite_system))] >>
               x3::repeat(19)[x3::char_];
      x3::parse(cnt.begin(), cnt.end(), r);
    } else if (label.compare("PGM / RUN BY / DATE") == 0) {
      std::string cnt = content_of(line);

      auto date_f = [&](auto const &ctx) {
        std::string all = x3::_attr(ctx), date_cnt = all.substr(0, 15),
                    zone = all.substr(15);
        boost::trim(zone);
        std::istringstream iss{date_cnt};
        iss >> std::chrono::parse("%Y%m%d %H%M%S", hdr.date);
        hdr.time_zone = time_zone(zone);
      };

      auto r = x3::repeat(20)[x3::char_][(STR(hdr.pgm))] >>
               x3::repeat(20)[x3::char_][(STR(hdr.run_by))] >>
               x3::repeat(20)[x3::char_][date_f];
      x3::parse(cnt.begin(), cnt.end(), r);
    } else if (label.compare("COMMENT") == 0) {
      std::string cnt = content_of(line);

      auto comments_f = [&](auto const &ctx) {
        hdr.comments.push_back(x3::_attr(ctx));
      };
      auto r = x3::repeat(60)[x3::char_][comments_f];
      x3::parse(cnt.begin(), cnt.end(), r);
    } else if (label.compare("IONOSPHERIC CORR") == 0) {
      std::string cnt = content_of(line);
      IonosphericCorrectionType type;
      std::vector<double> vals;

      auto r =
          x3::repeat(4)[x3::char_][(STRf(type, ionospheric_correction_type))] >>
          x3::char_ >>
          x3::repeat(4)[x3::repeat(12)[x3::char_][(Da(vals.push_back))]];
      x3::parse(cnt.begin(), cnt.end(), r);

      hdr.ionospheric_corr.insert_or_assign(type, vals);
    } else if (label.compare("END OF HEADER") == 0) {
      end_normally = true;
      break;
    } else if (label.compare("LEAP SECONDS") == 0) {
      std::string cnt = content_of(line);

#define INS_LP(LABEL)                                                          \
  [&](auto val_) { hdr.leap_seconds.insert_or_assign(LABEL, val_); }

      auto r = x3::repeat(6)[x3::char_][(Ia(INS_LP(tLS)))] >>
               x3::repeat(6)[x3::char_][(Ia(INS_LP(tLSF)))] >>
               x3::repeat(6)[x3::char_][(Ia(INS_LP(WN)))] >>
               x3::repeat(6)[x3::char_][(Ia(INS_LP(DN)))];
      x3::parse(cnt.begin(), cnt.end(), r);
    } else if (label.compare("TIME SYSTEM CORR") == 0) {
      gaa_assert(
          tcl_counter != 2,
          "Syntax Error: More than 2 lines for label `TIME SYSTEM CORR`");
      if (tcl_counter == 0) {
        tcl_counter = 1;

        std::string cnt = content_of(line);

        auto r =
            x3::repeat(4)[x3::char_]
                         [(STRf(hdr.time_corr_type, time_correction_type))] >>
            x3::char_ >> x3::repeat(17)[x3::char_][(D(hdr.a0))] >>
            x3::repeat(14)[x3::char_][(D(hdr.a1))] >>
            x3::repeat(7)[x3::char_][(I(hdr.T))] >>
            x3::repeat(5)[x3::char_][(I(hdr.W))];
        x3::parse(cnt.begin(), cnt.end(), r);

        hdr.S = Not_Given;
        hdr.utc = Not_Assigned;
        continue;
      } else if (tcl_counter == 1) {
        tcl_counter = 2;

        std::string cnt = content_of(line);

        auto r = x3::char_ >>
                 x3::repeat(5)[x3::char_][(STRf(hdr.S, augmentation_system))] >>
                 x3::char_ >>
                 x3::repeat(2)[x3::char_][(If(hdr.utc, utc_identifier))] >>
                 x3::char_;
        x3::parse(cnt.begin(), cnt.end(), r);
        continue;
      }
    } else {
      continue;
      gaa_assert(false,
                 std::format("unknown RINEX Header label(v302): `{}`", label));
    }
  }

  gaa_assert(end_normally, "missing label `END HEADER`");

  return hdr;
}

Nav_dat_section_bds parse_nav_dat_bds(std::istream &in,
                                      Nav_hdr_section const &hdr) {
  gaa_assert(in.good(), "input stream not good");

  Nav_dat_section_bds dat;
  Nav_ephemeris_bds eph;
  std::string line;
  int ring_counter = 0;

  while (std::getline(in, line)) {
    if (line.empty()) {
      continue;
    }

    switch (ring_counter++) {
    case 0: {
      auto sys_f = [&](auto const &ctx) {
        char sys_cnt = x3::_attr(ctx);
        auto sys = satellite_system(sys_cnt);
        gaa_assert(sys == hdr.satellite_system,
                   "Mismatch of satellite system in hdr and dat");
        eph.satellite_system = sys;
      };

      auto PRN_f = [&](auto const &ctx) { eph.PRN = x3::_attr(ctx); };

      auto year_f = [&](auto const &ctx) {
        int x = x3::_attr(ctx);
        eph.year = std::chrono::year(x);
      };

      auto month_f = [&](auto const &ctx) {
        uint x = x3::_attr(ctx);
        eph.month = std::chrono::month(x);
      };

      auto day_f = [&](auto const &ctx) {
        uint x = x3::_attr(ctx);
        eph.day = std::chrono::day(x);
      };

      auto hour_f = [&](auto const &ctx) {
        int x = x3::_attr(ctx);
        eph.hour = std::chrono::hours(x);
      };

      auto minute_f = [&](auto const &ctx) {
        int x = x3::_attr(ctx);
        eph.minute = std::chrono::minutes(x);
      };

      auto second_f = [&](auto const &ctx) {
        int x = x3::_attr(ctx);
        eph.second = std::chrono::seconds(x);
      };

      auto r = x3::char_[sys_f] >> x3::int_[PRN_f] >> x3::char_ >>
               x3::int_[year_f] >> x3::char_ >> x3::uint_[month_f] >>
               x3::char_ >> x3::uint_[day_f] >> x3::char_ >> x3::int_[hour_f] >>
               x3::char_ >> x3::int_[minute_f] >> x3::char_ >>
               x3::int_[second_f] >>
               x3::repeat(19)[x3::char_][(D(eph.clock_bias))] >>
               x3::repeat(19)[x3::char_][(D(eph.clock_drift))] >>
               x3::repeat(19)[x3::char_][(D(eph.clock_drift_rate))];
      x3::parse(line.begin(), line.end(), r);
      break;
    }
    case 1: {
      auto r = x3::repeat(4)[x3::char_] >>
               x3::repeat(19)[x3::char_][(D(eph.IODE))] >>
               x3::repeat(19)[x3::char_][(D(eph.Crs))] >>
               x3::repeat(19)[x3::char_][(D(eph.Delta_n))] >>
               x3::repeat(19)[x3::char_][(D(eph.M0))];
      x3::parse(line.begin(), line.end(), r);
      break;
    }
    case 2: {
      auto r = x3::repeat(4)[x3::char_] >>
               x3::repeat(19)[x3::char_][(D(eph.Cuc))] >>
               x3::repeat(19)[x3::char_][(D(eph.e_Eccentricity))] >>
               x3::repeat(19)[x3::char_][(D(eph.Cus))] >>
               x3::repeat(19)[x3::char_][(D(eph.sqrtA))];
      x3::parse(line.begin(), line.end(), r);
      break;
    }
    case 3: {
      auto r = x3::repeat(4)[x3::char_] >>
               x3::repeat(19)[x3::char_][(D(eph.Toe))] >>
               x3::repeat(19)[x3::char_][(D(eph.Cic))] >>
               x3::repeat(19)[x3::char_][(D(eph.OMEGA0))] >>
               x3::repeat(19)[x3::char_][(D(eph.negCis))];
      x3::parse(line.begin(), line.end(), r);
      eph.Cis = -eph.negCis;
      break;
    }
    case 4: {
      auto r = x3::repeat(4)[x3::char_] >>
               x3::repeat(19)[x3::char_][(D(eph.i0))] >>
               x3::repeat(19)[x3::char_][(D(eph.Crc))] >>
               x3::repeat(19)[x3::char_][(D(eph.omega))] >>
               x3::repeat(19)[x3::char_][(D(eph.OMEGA_DOT))];
      x3::parse(line.begin(), line.end(), r);
      break;
    }
    case 5: {
      auto r = x3::repeat(4)[x3::char_] >>
               x3::repeat(19)[x3::char_][(D(eph.IDOT))] >>
               x3::repeat(19)[x3::char_] >>
               x3::repeat(19)[x3::char_][(D(eph.BDT_Week))] >>
               x3::repeat(19)[x3::char_];
      x3::parse(line.begin(), line.end(), r);
      break;
    }
    case 6: {
      auto r = x3::repeat(4)[x3::char_] >>
               x3::repeat(19)[x3::char_][(D(eph.SV_accuracy))] >>
               x3::repeat(19)[x3::char_][(D(eph.SatH1))] >>
               x3::repeat(19)[x3::char_][(D(eph.TGD1))] >>
               x3::repeat(19)[x3::char_][(D(eph.TGD2))];
      x3::parse(line.begin(), line.end(), r);
      break;
    }
    case 7: {
      auto r = x3::repeat(4)[x3::char_] >>
               x3::repeat(19)[x3::char_][(D(eph.transmission_time))] >>
               x3::repeat(19)[x3::char_][(D(eph.IODC))] >>
               x3::repeat(19)[x3::char_] >> x3::repeat(19)[x3::char_];
      x3::parse(line.begin(), line.end(), r);
      ring_counter = 0;
      dat.push_back(eph);
      break;
    }

    default:
      gaa_assert(false, "unreachable default case");
    }
  }

  gaa_assert(ring_counter == 0, "Syntax Error: incomplete ephemeris");

  return dat;
}
} // namespace gaa::rinex::v302

#endif