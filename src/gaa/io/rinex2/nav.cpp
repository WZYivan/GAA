#include <filesystem>
#include <fstream>

#include <gaa/core/boost/spirit/x3.hpp>
#include <gaa/io/rinex2/nav.hpp>
#include <gaa/io/rinex3/nav.hpp>

namespace fs = std::filesystem;

namespace gaa {
std::string Rinex2_nav::Labels::version_type{"RINEX VERSION / TYPE"},
    Rinex2_nav::Labels::pgm_run_by{"PGM / RUN BY / DATE"},
    Rinex2_nav::Labels::comment{"COMMENT"},
    Rinex2_nav::Labels::ion_alpha{"ION ALPHA"},
    Rinex2_nav::Labels::ion_beta{"ION BETA"},
    Rinex2_nav::Labels::delta_utc{"DELTA-UTC"},
    Rinex2_nav::Labels::leap_secs{"LEAP SECONDS"},
    Rinex2_nav::Labels::end{"END OF HEADER"};

std::string Rinex2_nav::Fields::version{"version"},

    Rinex2_nav::Fields::type{"rinex type"};
std::string Rinex2_nav::Fields::pgm{"program"},
    Rinex2_nav::Fields::run_by{"runner"}, Rinex2_nav::Fields::date{"date"};
std::string Rinex2_nav::Fields::comments{"comments"};
std::string Rinex2_nav::Fields::ion_alpha{"iona"},
    Rinex2_nav::Fields::ion_beta{"ionb"};
std::string Rinex2_nav::Fields::a0{"a0"}, Rinex2_nav::Fields::a1{"a1"},
    Rinex2_nav::Fields::T{"T"}, Rinex2_nav::Fields::W{"W"};
std::string Rinex2_nav::Fields::leap_secs{"leap seconds"};

double Rinex2_nav::version() const {
  return m_table.meta_at<Tab_double>(Fields::version);
}
Rinex_type Rinex2_nav::type() const {
  return rinex_type_rinex3(m_table.meta_at<Tab_string>(Fields::type));
}

std::string const &Rinex2_nav::program() const {
  return m_table.meta_at<Tab_string>(Fields::pgm);
}
std::string const &Rinex2_nav::generator() const {
  return m_table.meta_at<Tab_string>(Fields::run_by);
}
std::string const &Rinex2_nav::date() const {
  return m_table.meta_at<Tab_string>(Fields::date);
}

std::vector<std::string> const &Rinex2_nav::comments() const {
  return m_table.meta_at<Tab_vecs>(Fields::comments);
}

std::vector<double> const &Rinex2_nav::iona() const {
  return m_table.meta_at<Tab_vecd>(Fields::ion_alpha);
}
std::vector<double> const &Rinex2_nav::ionb() const {
  return m_table.meta_at<Tab_vecd>(Fields::ion_beta);
}

double Rinex2_nav::polynomial_coeff_0() const {
  return m_table.meta_at<Tab_double>(Fields::a0);
}
double Rinex2_nav::polynomial_coeff_1() const {
  return m_table.meta_at<Tab_double>(Fields::a1);
}
int Rinex2_nav::reference_time_of_polynomial() const {
  return m_table.meta_at<Tab_int>(Fields::T);
}
int Rinex2_nav::reference_week_number() const {
  return m_table.meta_at<Tab_int>(Fields::W);
}

int Rinex2_nav::number_of_leap_seconds() const {
  return m_table.meta_at<Tab_int>(Fields::leap_secs);
}

Table &Rinex2_nav::table() { return m_table; }
Table const &Rinex2_nav::table() const { return m_table; }

Rinex2_nav read_rinex2_nav(std::string const &fname, kwargs args) {
  fs::path fpath{fname};
  gaa_assert(fs::exists(fpath), std::format("File not exists:{:s}", fname));
  std::ifstream ifs{fname, std::ios::in};
  return read_rinex2_nav(ifs, args);
}

Rinex2_nav read_rinex2_nav(std::istream &is, kwargs args) {
  gaa_assert(is.good());

  GAA_ARG_OR(args, trim_string, true);

  namespace x3 = ::gaa::spirit::x3;

  Rinex2_nav rinex;
  Table &table = rinex.table();
  std::string line;

  while (std::getline(is, line)) {
    if (line.empty()) {
      break;
    }

    boost::trim_right(line);

    auto body = gaa::substr(line, 0, 60);
    auto label = gaa::substr(line, 60);

    if (label == Rinex2_nav::Labels::version_type) {
      double version;
      std::string ftype, ssys;
      auto rule = x3::vchr<9 + 11>[x3::p2dbl(version)] >>
                  x3::vchr<1 + 19>[x3::p2str(ftype).trim(trim_string)];
      gaa_assert(x3::parse(body, rule));

      table.meta_ioa<Tab_double>(Rinex2_nav::Fields::version, version);
      table.meta_ioa<Tab_string>(Rinex2_nav::Fields::type, ftype);
    } else if (label == Rinex2_nav::Labels::pgm_run_by) {
      std::string program, run_by, date;
      auto rule = x3::vchr<20>[x3::p2str(program).trim(trim_string)] >>
                  x3::vchr<20>[x3::p2str(run_by).trim(trim_string)] >>
                  x3::vchr<20>[x3::p2str(date).trim(trim_string)];
      gaa_assert(x3::parse(body, rule));
      table.meta_ioa<Tab_string>(Rinex2_nav::Fields::pgm, program);
      table.meta_ioa<Tab_string>(Rinex2_nav::Fields::run_by, run_by);
      table.meta_ioa<Tab_string>(Rinex2_nav::Fields::date, date);

    } else if (label == Rinex2_nav::Labels::comment) {
      std::string cmt;
      auto rule = x3::vchr<60>[x3::p2str(cmt).trim(trim_string)];
      gaa_assert(x3::parse(body, rule));

      if (table.meta_has(Rinex2_nav::Fields::comments) &&
          table.meta_is<Tab_Vector_String>(Rinex2_nav::Fields::comments)) {
        table.meta_at<Tab_vecs>(Rinex2_nav::Fields::comments).push_back(cmt);
      } else {
        table.meta_ioa<Tab_vecs>(Rinex2_nav::Fields::comments, {cmt});
      }
    } else if (label == Rinex2_nav::Labels::ion_alpha) {
      std::vector<Tab_double> dbl;
      auto rule = x3::skip<2> >> x3::vchr<12>[x3::p2dbl(dbl)] >>
                  x3::vchr<12>[x3::p2dbl(dbl)] >>
                  x3::vchr<12>[x3::p2dbl(dbl)] >> x3::vchr<12>[x3::p2dbl(dbl)];
      gaa_assert(x3::parse(body, rule));

      table.meta_ioa<Tab_vecd>(Rinex2_nav::Fields::ion_alpha, std::move(dbl));
    } else if (label == Rinex2_nav::Labels::ion_beta) {
      std::vector<Tab_double> dbl;
      auto rule = x3::skip<2> >> x3::vchr<12>[x3::p2dbl(dbl)] >>
                  x3::vchr<12>[x3::p2dbl(dbl)] >>
                  x3::vchr<12>[x3::p2dbl(dbl)] >> x3::vchr<12>[x3::p2dbl(dbl)];
      gaa_assert(x3::parse(body, rule));

      table.meta_ioa<Tab_vecd>(Rinex2_nav::Fields::ion_beta, std::move(dbl));
    } else if (label == Rinex2_nav::Labels::delta_utc) {
      Tab_double a0, a1;
      Tab_int T, W;
      auto rule = x3::skip<3> >> x3::vchr<19>[x3::p2dbl(a0)] >>
                  x3::vchr<19>[x3::p2dbl(a1)] >> x3::vchr<9>[x3::p2int(T)] >>
                  x3::vchr<9>[x3::p2int(W)];
      gaa_assert(x3::parse(body, rule));
      table.meta_ioa<Tab_double>(Rinex2_nav::Fields::a0, a0);
      table.meta_ioa<Tab_double>(Rinex2_nav::Fields::a1, a1);
      table.meta_ioa<Tab_int>(Rinex2_nav::Fields::T, T);
      table.meta_ioa<Tab_int>(Rinex2_nav::Fields::W, W);

    } else if (label == Rinex2_nav::Labels::leap_secs) {
      Tab_int itg;
      auto rule = x3::vchr<6>[x3::p2int(itg)];
      gaa_assert(x3::parse(body, rule));

      table.meta_ioa<Tab_int>(Rinex2_nav::Fields::leap_secs, itg);
    } else if (label == Rinex2_nav::Labels::end) {
      break;
    } else {
      gaa_assert(false, std::format("Unknown label:{}", label));
    }
  }

  std::vector<Tab_int> prn, year, month, day, hour, minute;
  std::vector<Tab_double> second, sv_clock_bias, sv_clock_drift,
      sv_clock_drift_rate;

  std::vector<Tab_double> IODE, Crs, Delta_n, M0;

  std::vector<Tab_double> Cuc, e, Cus, sqrtA;

  std::vector<Tab_double> Toe, Cic, OMEGA0, Cis;

  std::vector<Tab_double> i0, Crc, omega, OMEGA_DOT;

  std::vector<Tab_double> IDOT, L2C, Week, L2P;

  std::vector<Tab_double> sv_accuracy, sv_health, TGD, IODC;

  std::vector<Tab_double> transmission_time, fit_interval;

  while (std::getline(is, line)) {
    if (line.empty()) {
      break;
    }

    {
      std::string sub;
      for (int i = 0; i != 7; ++i) {
        gaa_assert(std::getline(is, sub));
        boost::trim_right(sub);
        if (sub.size() < 79) {
          sub.resize(79, ' ');
        }
        gaa_assert(
            sub.size() <= 79,
            std::format("Rinex 2 unexpected line whose size > 79:\'{}\'", sub));
        line.append(std::move(sub));
      }

      boost::replace_all(line, "\r", "");
    }

    auto rule =
        x3::vchr<2>[x3::p2int(prn)] >> x3::vchr<1 + 2>[x3::p2int(year)] >>
        x3::vchr<1 + 2>[x3::p2int(month)] >> x3::vchr<1 + 2>[x3::p2int(day)] >>
        x3::vchr<1 + 2>[x3::p2int(hour)] >>
        x3::vchr<1 + 2>[x3::p2int(minute)] >> x3::vchr<5>[x3::p2dbl(second)] >>
        x3::vchr<19>[x3::p2dbl(sv_clock_bias)] >>
        x3::vchr<19>[x3::p2dbl(sv_clock_drift)] >>
        x3::vchr<19>[x3::p2dbl(sv_clock_drift_rate)] >>

        x3::vchr<3 + 19>[x3::p2dbl(IODE)] >> x3::vchr<19>[x3::p2dbl(Crs)] >>
        x3::vchr<19>[x3::p2dbl(Delta_n)] >> x3::vchr<19>[x3::p2dbl(M0)] >>

        x3::vchr<3 + 19>[x3::p2dbl(Cuc)] >> x3::vchr<19>[x3::p2dbl(e)] >>
        x3::vchr<19>[x3::p2dbl(Cus)] >> x3::vchr<19>[x3::p2dbl(sqrtA)] >>

        x3::vchr<3 + 19>[x3::p2dbl(Toe)] >> x3::vchr<19>[x3::p2dbl(Cic)] >>
        x3::vchr<19>[x3::p2dbl(OMEGA0)] >> x3::vchr<19>[x3::p2dbl(Cis)] >>

        x3::vchr<3 + 19>[x3::p2dbl(i0)] >> x3::vchr<19>[x3::p2dbl(Crc)] >>
        x3::vchr<19>[x3::p2dbl(omega)] >> x3::vchr<19>[x3::p2dbl(OMEGA_DOT)] >>

        x3::vchr<3 + 19>[x3::p2dbl(IDOT)] >> x3::vchr<19>[x3::p2dbl(L2C)] >>
        x3::vchr<19>[x3::p2dbl(Week)] >> x3::vchr<19>[x3::p2dbl(L2P)] >>

        x3::vchr<3 + 19>[x3::p2dbl(sv_accuracy)] >>
        x3::vchr<19>[x3::p2dbl(sv_health)] >> x3::vchr<19>[x3::p2dbl(TGD)] >>
        x3::vchr<19>[x3::p2dbl(IODC)] >>

        x3::vchr<3 + 19>[x3::p2dbl(transmission_time)] >>
        x3::vchr<19>[x3::p2dbl(fit_interval)] >> x3::skip<19> >> x3::skip<19>;
    gaa_assert(x3::parse(line, rule));
  }

  table.push_back(prn, "sat prn");
  table.push_back(
      year | std::views::transform([](auto x) { return x + 2000; }) | dbl_vec,
      "year");
  table.push_back(month, "month");
  table.push_back(day, "day");
  table.push_back(hour, "hour");
  table.push_back(minute, "minute");
  table.push_back(second, "second");
  table.push_back(sv_clock_bias, "clock bias");
  table.push_back(sv_clock_drift, "clock drift");
  table.push_back(sv_clock_drift_rate, "clock drift rate");

  table.push_back(IODE, "IODE");
  table.push_back(Crs, "C_rs");
  table.push_back(Delta_n, "Delta n");
  table.push_back(M0, "M_0");

  table.push_back(Cuc, "C_uc");
  table.push_back(e, "e");
  table.push_back(Cus, "C_us");
  table.push_back(sqrtA, "sqrtA");

  table.push_back(Toe, "t_oe");
  table.push_back(Cic, "C_ic");
  table.push_back(OMEGA0, "Omega_0");
  table.push_back(Cis, "C_is");

  table.push_back(i0, "i_0");
  table.push_back(Crc, "C_rc");
  table.push_back(omega, "omega");
  table.push_back(OMEGA_DOT, "OmegaDot");

  table.push_back(IDOT, "IDOT");
  table.push_back(L2C, "Codes L2");
  table.push_back(Week, "GPS Week");
  table.push_back(L2P, "L2 P flag");

  table.push_back(sv_accuracy, "SV accuracy");
  table.push_back(sv_health, "sv health");
  table.push_back(TGD, "TGD");
  table.push_back(IODC, "IODC");

  table.push_back(transmission_time, "transmission time");
  table.push_back(fit_interval, "fit interval");

  return rinex;
}
} // namespace gaa