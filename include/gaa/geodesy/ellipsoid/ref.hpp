#pragma once

#include <gaa/geodesy/ellipsoid/fwd.hpp>

namespace gaa {
class Ellipsoid_ref {
private:
  Ellipsoid const *m_ptr;

public:
  ~Ellipsoid_ref() = default;
  Ellipsoid_ref(Ellipsoid const &e);

  operator Ellipsoid const &() const;
  Ellipsoid const *operator->() const;
  Ellipsoid const &operator*() const;

  Ellipsoid const &ellipsoid() const;
  Ellipsoid_ref &rebind(Ellipsoid const &e);

  Ellipsoid_ref &operator=(Ellipsoid_ref const &bind);
  Ellipsoid_ref &operator=(Ellipsoid const &e);

  bool operator==(Ellipsoid_ref const &other);
  bool operator==(Ellipsoid const &e);
  friend bool operator==(Ellipsoid const &e, Ellipsoid_ref const &bind);
  bool operator!=(Ellipsoid_ref const &other);
  bool operator!=(Ellipsoid const &e);
  friend bool operator!=(Ellipsoid const &e, Ellipsoid_ref const &bind);

  bool is_null() const;

  static Ellipsoid_ref cgcs2000();
  static Ellipsoid_ref wgs84();
  static Ellipsoid_ref krassovsky();
  static Ellipsoid_ref null();
};
} // namespace gaa