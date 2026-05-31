#include <gaa/wrap/std/any/any.hpp>
#include <gaa/wrap/std/any/ctrl.hpp>
#include <gaa/wrap/std/any/vtable.hpp>

namespace gaa {
namespace any {
bool is_null(Any const &any) { return any.is_null(); }

Vtable const &Any::vtable() const { return m_vtable.get(); }

std::string Any::format(std::string_view fmt) const {
  return this->vtable().format(*this, fmt);
}

std::type_info const &Any::type_info() const {
  return this->vtable().type_info();
}

bool Any::is_null() const { return this->is<std::nullptr_t>(); }

void Any::invoke(std::string const &key, Any const &input, void *ret) const {
  gaa_assert(vtable().plugins.contains(key), "no plugin named {}", key);
  vtable().plugins.at(key)(*this, input, ret);
}

Any::Info const &Any::info() const { return this->m_info; }

} // namespace any
} // namespace gaa