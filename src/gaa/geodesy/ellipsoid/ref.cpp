#include <gaa/geodesy/ellipsoid.hpp>

namespace gaa {

Ellipsoid_ref::Ellipsoid_ref(Ellipsoid const &e) : m_ptr(std::addressof(e)) {}

Ellipsoid const &Ellipsoid_ref::ellipsoid() const { return *m_ptr; }
Ellipsoid_ref::operator Ellipsoid const &() const { return this->ellipsoid(); }
Ellipsoid const *Ellipsoid_ref::operator->() const { return m_ptr; }
Ellipsoid const &Ellipsoid_ref::operator*() const { return this->ellipsoid(); }

Ellipsoid_ref &Ellipsoid_ref::rebind(Ellipsoid const &e) {
  m_ptr = std::addressof(e);
  return *this;
}
Ellipsoid_ref &Ellipsoid_ref::operator=(Ellipsoid_ref const &bind) {
  return this->rebind(bind);
}
Ellipsoid_ref &Ellipsoid_ref::operator=(Ellipsoid const &e) {
  return this->rebind(e);
}

bool Ellipsoid_ref::is_null() const { return this->ellipsoid().is_null(); }
Ellipsoid_ref Ellipsoid_ref::cgcs2000() { return Ellipsoid_ref(gaa::cgcs2000); }
Ellipsoid_ref Ellipsoid_ref::wgs84() { return Ellipsoid_ref(gaa::wgs84); }
Ellipsoid_ref Ellipsoid_ref::krassovsky() {
  return Ellipsoid_ref(gaa::krassovsky);
}
Ellipsoid_ref Ellipsoid_ref::null() {
  return Ellipsoid_ref(gaa::null_ellipsoid);
}

bool Ellipsoid_ref::operator==(Ellipsoid_ref const &other) {
  return this->ellipsoid() == other.ellipsoid();
}
bool Ellipsoid_ref::operator==(Ellipsoid const &e) {
  return this->ellipsoid() == e;
}
bool operator==(Ellipsoid const &e, Ellipsoid_ref const &bind) {
  return bind.ellipsoid() == e;
}
bool Ellipsoid_ref::operator!=(Ellipsoid_ref const &other) {
  return !(*this == other);
}
bool Ellipsoid_ref::operator!=(Ellipsoid const &e) { return !(*this == e); }
bool operator!=(Ellipsoid const &e, Ellipsoid_ref const &bind) {
  return !(bind == e);
}

} // namespace gaa