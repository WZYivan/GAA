#pragma once

#include <gaa/core/table.hpp>
#include <gaa/core/time.hpp>
#include <gaa/geodesy/space_rectangular.hpp>
#include <gaa/gnss/space/sv_orbit.hpp>

namespace gaa {

struct Param_sv_pos_from_broadcast {
  double sqrtA;
  double e;
  double t_oe;
  double M_0;
  double omega;
  double i_0;
  double IDOT;
  double C_us;
  double C_uc;
  double C_is;
  double C_ic;
  double C_rc;
  double C_rs;
  double Omega_0;
  double OmegaDot;
  double t;

  static Param_sv_pos_from_broadcast from_table_row(Table_row_ref row,
                                                    double t);
  static Param_sv_pos_from_broadcast from_table_row(Table_row_view row,
                                                    double t);
};

extern Space_rectangular_coordinate
sv_pos_from_broadcast(double sqrtA, double e, double t_oe, double M_0,
                      double omega, double i_0, double IDOT, double C_us,
                      double C_uc, double C_is, double C_ic, double C_rc,
                      double C_rs, double Omega_0, double OmegaDot, double t);
extern Space_rectangular_coordinate
sv_pos_from_broadcast(Param_sv_pos_from_broadcast const &param);
} // namespace gaa