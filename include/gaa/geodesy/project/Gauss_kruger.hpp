#pragma once

#include <gaa/geodesy/ellipsoids.hpp>

namespace gaa {
struct Gauss_kruger_project_coordinate;

struct _gauss_kruger_project_send_package_t {
  Gauss_kruger_project_coordinate const &gauss_kruger_coordinate;

  ~_gauss_kruger_project_send_package_t() = default;
  _gauss_kruger_project_send_package_t(
      Gauss_kruger_project_coordinate const &gkc)
      : gauss_kruger_coordinate(gkc) {}
};

class Gauss_kruger_project {
public:
  enum class Identifier { Three, Six };

private:
  int m_band; // custom project band code
  Identifier m_id;

public:
  ~Gauss_kruger_project() = default;
  Gauss_kruger_project(double val) : Gauss_kruger_project(val, -1) {}
  Gauss_kruger_project(double val, int band);
  Gauss_kruger_project(Identifier id, int band) : m_band(band), m_id(id) {}

  friend struct Gauss_kruger_project_coordinate;

  bool operator==(Gauss_kruger_project const &other) const;
  bool operator!=(Gauss_kruger_project const &other) const;

  Gauss_kruger_project_coordinate coordinate(double x, double y, int b,
                                             Ellipsoid const &e) const;
  Identifier interval() const;
  int band() const;
  bool is_virtual() const;
  int band_of(double lon) const;
  double center_meridian(double band) const;

  Gauss_kruger_project_coordinate eval(Geodetic_coordinate const &gc) const;
  Geodetic_coordinate reval(Gauss_kruger_project_coordinate const &pc) const;

  Gauss_kruger_project operator()(int band) const;
};

struct Gauss_kruger_project_coordinate {
  int band; // which band is this coordinate lies in
  double x, y;
  Gauss_kruger_project
      project; // its band will be modified ti keep same as this->band
  Ellipsoid const &ellipsoid;

  ~Gauss_kruger_project_coordinate() = default;

  Gauss_kruger_project_coordinate(double xp, double yp, int b,
                                  Gauss_kruger_project const &p,
                                  Ellipsoid const &e)
      : band(b), x(xp), y(yp), project(p), ellipsoid(e) {
    project.m_band = b;
  }

  double zone_y() const;
};

GAA_channel_begin((Geodetic_coordinate), (Gauss_kruger_project));
GAA_channel_acknowledge(s [[maybe_unused]], a [[maybe_unused]]) { return true; }
GAA_channel_eval((Gauss_kruger_project_coordinate), s, a) { return a.eval(s); }
GAA_channel_end();

GAA_channel_begin((Gauss_kruger_project_coordinate), (Ellipsoid));
GAA_channel_acknowledge(s, a) { return s.ellipsoid == a; }
GAA_channel_eval((Geodetic_coordinate), s, a [[maybe_unused]]) {
  return s.project.reval(s);
}
GAA_channel_end();

GAA_channel_begin((Gauss_kruger_project_coordinate), (Gauss_kruger_project));
GAA_channel_acknowledge(s, a) { return s.project.interval() == a.interval(); }
GAA_channel_eval((Gauss_kruger_project_coordinate), s, a) {
  return a.eval(s.project.reval(s));
}
GAA_channel_end();

template <> struct Geodetic_coordinate_caster<Gauss_kruger_project_coordinate> {
  static Geodetic_coordinate cast(Gauss_kruger_project_coordinate const &pc) {
    return pc.project.reval(pc);
  }

  static Geodetic_coordinate cast_to(Gauss_kruger_project_coordinate const &pc,
                                     Ellipsoid const &e) {
    return pc >> e;
  }
};

struct Gauss_kruger {
public:
  Gauss_kruger_project project(double interval) const;
  Gauss_kruger_project project(double interval, int band) const;
};

extern Gauss_kruger const GAA_API gauss_kruger;
} // namespace gaa