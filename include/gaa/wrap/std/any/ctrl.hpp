#pragma once

#include <typeindex>

#include <gaa/core/keywords.hpp>
#include <gaa/wrap/std/any/vtable.hpp>

namespace gaa {
namespace any {
class Vtable_ctrl : public std::map<std::type_index, Vtable> {
private:
  using Base = std::map<std::type_index, Vtable>;

public:
  Vtable_ctrl() = default;
  ~Vtable_ctrl() = default;

  /// auto controlled scope
  template <class T> std::type_index index() { return typeid(std::decay_t<T>); }

  template <class T> void init() {
    if (!this->has<T>()) {
      Base::insert_or_assign(this->index<T>(), Vtable::init<T>());
    }
  }

  template <class T> bool has() { return Base::contains(this->index<T>()); }

  template <class T> Vtable const &get() const {
    gaa_assert(this->has<T>());
    return Base::at(this->index<T>());
  }

  template <class T> Vtable &get() {
    if (!this->has<T>()) {
      this->init<T>();
    }
    return Base::at(this->index<T>());
  }
  /// auto controlled scope

  /// register your plugin, managed by user, not this framework
  template <class T, class F> void new_plugin(std::string const &key, F &&f) {
    Vtable &vtable = this->get<T>();
    vtable.plugins.insert_or_assign(key, f);
  }
};

/// get the global instance of Vtable
inline Vtable_ctrl &vtable_ctrl() {
  static Vtable_ctrl instance;
  return instance;
}
// extern Vtable_ctrl ctrl;
} // namespace any
} // namespace gaa