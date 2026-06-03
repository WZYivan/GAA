#include <gaa/wrap/std/any/any.hpp>
#include <gaa/wrap/std/any/ctrl.hpp>
#include <gaa/wrap/std/any/vtable.hpp>

namespace gaa {
namespace any {
Any::Any(Any const &other)
    : Base(dynamic_cast<std::any const &>(other)), m_vtable(other.m_vtable),
      m_info(other.m_info) {}
Any::Any(Any &&other)
    : Base(dynamic_cast<std::any &&>(other)),
      m_vtable(std::move(other.m_vtable)), m_info(std::move(other.m_info)) {}
Any &Any::operator=(Any const &other) {
  Base::operator=(dynamic_cast<std::any const &>(other));
  m_vtable = other.m_vtable;
  m_info = other.m_info;
  return *this;
}
Any &Any::operator=(Any &&other) {
  Base::operator=(dynamic_cast<std::any &&>(other));
  m_vtable = std::move(other.m_vtable);
  m_info = std::move(other.m_info);
  return *this;
}

Vtable const &Any::vtable() const { return m_vtable.get(); }
bool Any::is_null() const { return this->is<std::nullptr_t>(); }
Any::Info const &Any::info() const { return this->m_info; }

std::string Any::format(std::string_view fmt) const {
  return this->vtable().format(*this, fmt);
}
std::type_info const &Any::type_info() const {
  return this->vtable().type_info();
}
void Any::for_each(Callback callback) const {
  return this->vtable().for_each(*this, callback);
}
void Any::push_back_literal(std::string const &cnt) {
  return this->vtable().push_back_literal(*this, cnt);
}
void Any::push_back_literal(std::string const &cnt, Literal_Type lt) {
  return this->vtable().push_back_literal_2(*this, cnt, lt);
}
std::size_t Any::size() const { return this->vtable().size(*this); }

void Any::invoke(std::string const &key, void *input, void *output) const {
  gaa_assert(vtable().plugins.contains(key), "no plugin named {}", key);
  vtable().plugins.at(key)(*this, input, output);
}

} // namespace any
} // namespace gaa