#pragma once

#include <gaa/container/table.hpp>
#include <gaa/core/kw.hpp>
#include <gaa/gnss/def.hpp>

namespace gaa {
class Rinex2_nav {
public:
  struct Labels {
    static std::string version_type, pgm_run_by, comment, ion_alpha, ion_beta,
        delta_utc, leap_secs, end;
  };

  struct Fields {
    static std::string version, type;
    static std::string pgm, run_by, date;
    static std::string comments;
    static std::string ion_alpha, ion_beta;
    static std::string a0, a1, T, W;
    static std::string leap_secs;
  };

private:
  Table m_table;

public:
  ~Rinex2_nav() = default;
  Rinex2_nav() = default;

  double version() const;
  Rinex_type type() const;

  std::string const &program() const;
  std::string const &generator() const;
  std::string const &date() const;

  std::vector<std::string> const &comments() const;

  std::vector<double> const &iona() const;
  std::vector<double> const &ionb() const;

  double polynomial_coeff_0() const;
  double polynomial_coeff_1() const;
  int reference_time_of_polynomial() const;
  int reference_week_number() const;

  int number_of_leap_seconds() const;

  Table &table();
  Table const &table() const;
  std::string glimpse() const;
};

extern Rinex2_nav read_rinex2_nav(std::string const &fname, kwargs args = {});
extern Rinex2_nav read_rinex2_nav(std::istream &is, kwargs args = {});
} // namespace gaa