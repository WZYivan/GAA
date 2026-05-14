#pragma once

#include <gaa/core/kw.hpp>
#include <gaa/core/table.hpp>
#include <gaa/gnss/def.hpp>

namespace gaa {

extern Satellite_System sat_sys_rinex3(std::string const &cnt);
extern Satellite_System sat_sys_rinex3(char cnt);
extern Rinex_type rinex_type_rinex3(std::string const &cnt);
extern UTC_Identifier utc_id_rinex3(int cnt);

class Rinex3 {
public:
  struct Labels {
    static std::string version_type, pgm_run_by, comment, iono_corr, time_corr,
        leap_secs, end;
  };

  struct Fields {
    static std::string version, sat_sys, type;
    static std::string pgm, run_by, date;
    static std::string comments;
    static std::string bdsa, bdsb;
    static std::string time_corr_type, a0, a1, T, W, utc_id, S;
    static std::string tLS, tLSF, WN, DN;
  };

private:
  Table m_table;

public:
  Rinex3() = default;
  ~Rinex3() = default;

  double version() const;
  Satellite_System sat_sys() const;
  Rinex_type type() const;

  std::string const &program() const;
  std::string const &generator() const;
  std::string const &date() const;

  std::vector<std::string> const &comments() const;

  std::vector<double> const &bdsa() const;
  std::vector<double> const &bdsb() const;

  std::string const &time_corr_type() const;
  double polynomial_coeff_0() const;
  double polynomial_coeff_1() const;
  int reference_time_of_polynomial() const;
  int reference_week_number() const;
  UTC_Identifier utc_id() const;

  int number_of_leap_seconds() const;
  int future_or_past_leap_second() const;
  int respective_week_number() const;
  int respective_day_number() const;

  std::string glimpse() const;

  Table_row_view row(std::size_t row) const;

  friend Rinex3 read_rinex3(std::string const &fname, kwargs args);
  friend Rinex3 read_rinex3(std::istream &is, kwargs args);
};

extern Rinex3 read_rinex3(std::string const &fname, kwargs args = {});
extern Rinex3 read_rinex3(std::istream &is, kwargs args = {});
} // namespace gaa