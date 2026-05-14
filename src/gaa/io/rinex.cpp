#include <filesystem>
#include <fstream>

#include <gaa/core/boost/spirit/x3.hpp>
#include <gaa/io/rinex.hpp>

namespace fs = std::filesystem;

namespace gaa {

Satellite_System sat_sys_rinex3(std::string const &cnt) {
  if (cnt.starts_with('C')) {
    return BDS;
  } else if (cnt.starts_with('G')) {
    return GPS;
  } else if (cnt.starts_with('R')) {
    return GLONASS;
  } else {
    return Satellite_System_UNKNOWN;
  }
}

Satellite_System sat_sys_rinex3(char cnt) {
  switch (cnt) {
  case 'C':
    return BDS;
  case 'G':
    return GPS;
  case 'R':
    return GLONASS;
  default:
    gaa_assert(false, "unreachable default case");
  }
}

Rinex_type rinex_type_rinex3(std::string const &cnt) {
  if (cnt.starts_with('N')) {
    return Rinex_Navigation;
  } else {
    return Rinex_type_UNKNOWN;
  }
}

UTC_Identifier utc_id_rinex3(int cnt) {
  gaa_assert(cnt > 0 && cnt < UTC_Identifier_COUNT);
  return static_cast<UTC_Identifier>(cnt);
}

std::string Rinex3::Labels::version_type{"RINEX VERSION / TYPE"},
    Rinex3::Labels::pgm_run_by{"PGM / RUN BY / DATE"},
    Rinex3::Labels::comment{"COMMENT"},
    Rinex3::Labels::iono_corr{"IONOSPHERIC CORR"},
    Rinex3::Labels::time_corr{"TIME SYSTEM CORR"},
    Rinex3::Labels::leap_secs{"LEAP SECONDS"},
    Rinex3::Labels::end{"END OF HEADER"};

std::string Rinex3::Fields::version{"version"},
    Rinex3::Fields::sat_sys{"satellite system"},
    Rinex3::Fields::type{"rinex type"};
std::string Rinex3::Fields::pgm{"program"}, Rinex3::Fields::run_by{"runner"},
    Rinex3::Fields::date{"date"};
std::string Rinex3::Fields::comments{"comments"};
std::string Rinex3::Fields::time_corr_type{"time corr type"},
    Rinex3::Fields::bdsa{"BDSA"}, Rinex3::Fields::bdsb{"BDSB"};
std::string Rinex3::Fields::a0{"a0"}, Rinex3::Fields::a1{"a1"},
    Rinex3::Fields::T{"T"}, Rinex3::Fields::W{"W"},
    Rinex3::Fields::utc_id{"UTC ID"}, Rinex3::Fields::S{"S"};
std::string Rinex3::Fields::tLS{"tLS"}, Rinex3::Fields::tLSF{"tLSF"},
    Rinex3::Fields::WN{"WN"}, Rinex3::Fields::DN{"DN"};

std::string Rinex3::glimpse() const { return m_table.glimpse(); }

Table_row_view Rinex3::row(std::size_t row) const { return m_table.row(row); }

double Rinex3::version() const {
  return m_table.meta_at<Tab_double>(Fields::version);
}
Satellite_System Rinex3::sat_sys() const {
  return sat_sys_rinex3(m_table.meta_at<Tab_string>(Fields::sat_sys));
}
Rinex_type Rinex3::type() const {
  return rinex_type_rinex3(m_table.meta_at<Tab_string>(Fields::type));
}

std::string const &Rinex3::program() const {
  return m_table.meta_at<Tab_string>(Fields::pgm);
}
std::string const &Rinex3::generator() const {
  return m_table.meta_at<Tab_string>(Fields::run_by);
}
std::string const &Rinex3::date() const {
  return m_table.meta_at<Tab_string>(Fields::date);
}

std::vector<std::string> const &Rinex3::comments() const {
  return m_table.meta_at<Tab_vecs>(Fields::comments);
}

std::vector<double> const &Rinex3::bdsa() const {
  return m_table.meta_at<Tab_vecd>(Fields::bdsa);
}
std::vector<double> const &Rinex3::bdsb() const {
  return m_table.meta_at<Tab_vecd>(Fields::bdsb);
}

std::string const &Rinex3::time_corr_type() const {
  return m_table.meta_at<Tab_string>(Fields::time_corr_type);
}
double Rinex3::polynomial_coeff_0() const {
  return m_table.meta_at<Tab_int>(Fields::a0);
}
double Rinex3::polynomial_coeff_1() const {
  return m_table.meta_at<Tab_int>(Fields::a1);
}
int Rinex3::reference_time_of_polynomial() const {
  return m_table.meta_at<Tab_int>(Fields::T);
}
int Rinex3::reference_week_number() const {
  return m_table.meta_at<Tab_int>(Fields::W);
}
UTC_Identifier Rinex3::utc_id() const {
  return utc_id_rinex3(m_table.meta_at<Tab_int>(Fields::utc_id));
}

int Rinex3::number_of_leap_seconds() const {
  return m_table.meta_at<Tab_int>(Fields::tLS);
}
int Rinex3::future_or_past_leap_second() const {
  return m_table.meta_at<Tab_int>(Fields::tLSF);
}
int Rinex3::respective_week_number() const {
  return m_table.meta_at<Tab_int>(Fields::WN);
}
int Rinex3::respective_day_number() const {
  return m_table.meta_at<Tab_int>(Fields::DN);
}

void _read_rinex3_bds_data(std::istream &is, Table &table) {
  namespace x3 = ::gaa::spirit::x3;

  std::vector<Tab_char> sys;
  std::vector<Tab_int> prn, year, month, day, hour, minute, second;
  std::vector<Tab_double> sv_clock_bias, sv_clock_drift, sv_clock_drift_rate;

  std::vector<Tab_double> IODE, Crs, Delta_n, M0;

  std::vector<Tab_double> Cuc, e, Cus, sqrtA;

  std::vector<Tab_double> Toe, Cic, OMEGA0, Cis;

  std::vector<Tab_double> i0, Crc, omega, OMEGA_DOT;

  std::vector<Tab_double> IDOT, L2C, Week, L2P;

  std::vector<Tab_double> sv_accuracy, sv_health, TGD, IODC;

  std::vector<Tab_double> transmission_time, fit_interval;

  std::string line;
  while (std::getline(is, line)) {
    if (line.empty()) {
      break;
    }

    {
      std::string sub;
      for (int i = 0; i != 7; ++i) {
        gaa_assert(std::getline(is, sub));
        line.append(std::move(sub));
      }

      boost::replace_all(line, "\r", "");
    }

    gaa_assert(line.size() == 640,
               "Size of each line in Rinex3(BDS) data record must be 80");

    auto rule =
        x3::vchr<1>[x3::p2chr(sys)] >> x3::vchr<2>[x3::p2int(prn)] >>
        x3::vchr<1 + 4>[x3::p2int(year)] >> x3::vchr<1 + 2>[x3::p2int(month)] >>
        x3::vchr<1 + 2>[x3::p2int(day)] >> x3::vchr<1 + 2>[x3::p2int(hour)] >>
        x3::vchr<1 + 2>[x3::p2int(minute)] >>
        x3::vchr<1 + 2>[x3::p2int(second)] >>
        x3::vchr<19>[x3::p2dbl(sv_clock_bias)] >>
        x3::vchr<19>[x3::p2dbl(sv_clock_drift)] >>
        x3::vchr<19>[x3::p2dbl(sv_clock_drift_rate)] >>

        x3::vchr<4 + 19>[x3::p2dbl(IODE)] >> x3::vchr<19>[x3::p2dbl(Crs)] >>
        x3::vchr<19>[x3::p2dbl(Delta_n)] >> x3::vchr<19>[x3::p2dbl(M0)] >>

        x3::vchr<4 + 19>[x3::p2dbl(Cuc)] >> x3::vchr<19>[x3::p2dbl(e)] >>
        x3::vchr<19>[x3::p2dbl(Cus)] >> x3::vchr<19>[x3::p2dbl(sqrtA)] >>

        x3::vchr<4 + 19>[x3::p2dbl(Toe)] >> x3::vchr<19>[x3::p2dbl(Cic)] >>
        x3::vchr<19>[x3::p2dbl(OMEGA0)] >> x3::vchr<19>[x3::p2dbl(Cis)] >>

        x3::vchr<4 + 19>[x3::p2dbl(i0)] >> x3::vchr<19>[x3::p2dbl(Crc)] >>
        x3::vchr<19>[x3::p2dbl(omega)] >> x3::vchr<19>[x3::p2dbl(OMEGA_DOT)] >>

        x3::vchr<4 + 19>[x3::p2dbl(IDOT)] >> x3::vchr<19>[x3::p2dbl(L2C)] >>
        x3::vchr<19>[x3::p2dbl(Week)] >> x3::vchr<19>[x3::p2dbl(L2P)] >>

        x3::vchr<4 + 19>[x3::p2dbl(sv_accuracy)] >>
        x3::vchr<19>[x3::p2dbl(sv_health)] >> x3::vchr<19>[x3::p2dbl(TGD)] >>
        x3::vchr<19>[x3::p2dbl(IODC)] >>

        x3::vchr<4 + 19>[x3::p2dbl(transmission_time)] >>
        x3::vchr<19>[x3::p2dbl(fit_interval)] >> x3::skip<19> >> x3::skip<19>;
    gaa_assert(x3::parse(line, rule));
  }

  table.push_back(sys | std::views::transform([](char c) {
                    return sat_sys_rinex3(c);
                  }) | std::ranges::to<std::vector<Tab_sat_sys>>(),
                  "sat sys");
  table.push_back(prn, "sat prn");
  table.push_back(year, "year");
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
  table.push_back(L2C, "Coeds L2");
  table.push_back(Week, "GPS Week");
  table.push_back(L2P, "L2 P flag");

  table.push_back(sv_accuracy, "SV accuracy");
  table.push_back(sv_health, "sv health");
  table.push_back(TGD, "TGD");
  table.push_back(IODC, "IODC");

  table.push_back(transmission_time, "transmission time");
  table.push_back(fit_interval, "fit interval");
}

Rinex3 read_rinex3(std::string const &fname, kwargs args) {
  fs::path fpath{fname};
  gaa_assert(fs::exists(fpath), std::format("File not exists:{:s}", fname));
  std::ifstream ifs{fname, std::ios::in};
  return read_rinex3(ifs, args);
}

Rinex3 read_rinex3(std::istream &is, kwargs args) {
  gaa_assert(is.good());

  GAA_ARG_OR(args, trim_string, true);

  namespace x3 = gaa::spirit::x3;

  Rinex3 rinex3;
  Table &table = rinex3.m_table;
  std::string line;

  int time_sys_corr_stage = 0;
  while (std::getline(is, line)) {
    if (line.empty()) {
      break;
    }

    if (line.ends_with('\r')) {
      line.pop_back();
    }

    auto body = gaa::substr(line, 0, 60);
    auto label = gaa::substr(line, 60);

    if (label == Rinex3::Labels::version_type) {
      double version;
      std::string ftype, ssys;
      auto rule = x3::vchr<9 + 11>[x3::p2dbl(version)] >>
                  x3::vchr<1 + 19>[x3::p2str(ftype).trim(trim_string)] >>
                  x3::vchr<1 + 19>[x3::p2str(ssys).trim(trim_string)];
      gaa_assert(x3::parse(body, rule));

      table.meta_ioa<Tab_double>(Rinex3::Fields::version, version);
      table.meta_ioa<Tab_string>(Rinex3::Fields::type, ftype);
      table.meta_ioa<Tab_string>(Rinex3::Fields::sat_sys, ssys);

    } else if (label == Rinex3::Labels::pgm_run_by) {
      std::string program, run_by, date;
      auto rule = x3::vchr<20>[x3::p2str(program).trim(trim_string)] >>
                  x3::vchr<20>[x3::p2str(run_by).trim(trim_string)] >>
                  x3::vchr<20>[x3::p2str(date).trim(trim_string)];
      gaa_assert(x3::parse(body, rule));
      table.meta_ioa<Tab_string>(Rinex3::Fields::pgm, program);
      table.meta_ioa<Tab_string>(Rinex3::Fields::run_by, run_by);
      table.meta_ioa<Tab_string>(Rinex3::Fields::date, date);

    } else if (label == Rinex3::Labels::comment) {
      std::string cmt;
      auto rule = x3::vchr<60>[x3::p2str(cmt).trim(trim_string)];
      gaa_assert(x3::parse(body, rule));

      if (table.meta_has(Rinex3::Fields::comments) &&
          table.meta_is<Tab_Vector_String>(Rinex3::Fields::comments)) {
        table.meta_at<Tab_vecs>(Rinex3::Fields::comments).push_back(cmt);
      } else {
        table.meta_ioa<Tab_vecs>(Rinex3::Fields::comments, {cmt});
      }

    } else if (label == Rinex3::Labels::iono_corr) {
      std::string corr_type;
      Tab_vecd vecd;

      auto rule =
          x3::vchr<4 + 1>[x3::p2str(corr_type).trim(trim_string)] >>
          x3::vchr<12>[x3::p2dbl(vecd)] >> x3::vchr<12>[x3::p2dbl(vecd)] >>
          x3::vchr<12>[x3::p2dbl(vecd)] >> x3::vchr<12>[x3::p2dbl(vecd)];
      gaa_assert(x3::parse(body, rule));

      table.meta_ioa<Tab_vecd>(corr_type, vecd);

    } else if (label == Rinex3::Labels::time_corr) {
      if (time_sys_corr_stage == 0) {
        std::string corr_type;
        double a0, a1;
        int T, W;

        auto rule = x3::vchr<4 + 1>[x3::p2str(corr_type)] >>
                    x3::vchr<17>[x3::p2dbl(a0)] >>
                    x3::vchr<16>[x3::p2dbl(a1)] >> x3::vchr<7>[x3::p2int(T)] >>
                    x3::vchr<5>[x3::p2int(W)];
        gaa_assert(x3::parse(body, rule));

        table.meta_ioa<Tab_string>(Rinex3::Fields::time_corr_type, corr_type);
        table.meta_ioa<Tab_double>(Rinex3::Fields::a0, a0);
        table.meta_ioa<Tab_double>(Rinex3::Fields::a1, a1);
        table.meta_ioa<Tab_int>(Rinex3::Fields::T, T);
        table.meta_ioa<Tab_int>(Rinex3::Fields::W, W);

        time_sys_corr_stage++;
      } else if (time_sys_corr_stage == 1) {
        int utc_id;
        std::string S;

        auto rule = x3::vchr<1 + 5 + 1>[x3::p2str(S)] >>
                    x3::vchr<2 + 1>[x3::p2int(utc_id)];
        gaa_assert(x3::parse(body, rule));

        table.meta_ioa<Tab_string>(Rinex3::Fields::S, S);
        table.meta_ioa<Tab_int>(Rinex3::Fields::utc_id, utc_id);

      } else {
        gaa_assert(false, "Get unexpected 3 line of TIME SYSTEM CORR");
      }

    } else if (label == Rinex3::Labels::leap_secs) {
      int tLS, tLSF, WN_LSF, DN;

      auto rule = x3::vchr<6>[x3::p2int(tLS)] >> x3::vchr<6>[x3::p2int(tLSF)] >>
                  x3::vchr<6>[x3::p2int(WN_LSF)] >> x3::vchr<6>[x3::p2int(DN)];
      gaa_assert(x3::parse(body, rule));

      table.meta_ioa<Tab_int>(Rinex3::Fields::tLS, tLS);
      table.meta_ioa<Tab_int>(Rinex3::Fields::tLSF, tLSF);
      table.meta_ioa<Tab_int>(Rinex3::Fields::WN, WN_LSF);
      table.meta_ioa<Tab_int>(Rinex3::Fields::DN, DN);

    } else if (label == Rinex3::Labels::end) {
      break;
    } else {
      gaa_assert(false, std::format("Unknown label:{}", label));
    }
  }

  gaa_assert(table.meta_has("satellite system"),
             "File format error: Rinex3 lack of required field");

  switch (sat_sys_rinex3(table.meta_at<Tab_string>("satellite system"))) {
  case BDS: {
    _read_rinex3_bds_data(is, table);
    break;
  }
  default: {
    gaa_assert(false, "unreachable default case");
  }
  }

  return rinex3;
}
} // namespace gaa