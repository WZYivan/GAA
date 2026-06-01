#pragma once

#include <gaa/wrap/std/any/ctrl.hpp>
#include <gaa/wrap/std/any/vtable.hpp>
#include <gaa/wrap/std/concepts.hpp>

namespace gaa {
namespace any {
class Any : public std::any {
public:
  struct Info {
    bool is_scalar;
    bool is_string;
    bool is_vector;
    bool is_map;
    bool is_idx_map;
    bool is_str_map;

    template <class T> static Info init() {
      using D = std::decay_t<T>;
      return Info{.is_scalar = is_scalar_v<D>,
                  .is_string = is_string_v<D>,
                  .is_vector = is_vector_v<D>,
                  .is_map = is_map_v<D>,
                  .is_idx_map = is_idx_map_v<D>,
                  .is_str_map = is_str_map_v<D>};
    }
  };

private:
  using Base = std::any;

  std::reference_wrapper<Vtable const> m_vtable;
  Info m_info;

public:
  ~Any() = default;

  template <class T>
    requires(!std::same_as<Any, std::decay_t<T>>)
  Any(T &&obj)
      : Base(obj), m_vtable(vtable_ctrl().get<T>()), m_info(Info::init<T>()) {}

  template <class T>
    requires(!is_pair_v<T>)
  Any(std::initializer_list<T> il)
      : Base(std::vector<T>(il)), m_vtable(vtable_ctrl().get<std::vector<T>>()),
        m_info(Info::init<std::vector<T>>()) {}

  Any(char const *chars) : Any(std::string(chars)) {}

  Any(Any const &other);
  Any(Any &&other);
  Any &operator=(Any const &other);
  Any &operator=(Any &&other);

  template <class T>
    requires(!std::same_as<Any, std::decay_t<T>>)
  Any &operator=(T &&obj) {
    Base::operator=(obj);
    this->m_vtable = std::cref(vtable_ctrl().get<T>());
    this->m_info = Info::init<T>();
    return *this;
  }

  template <class T>
    requires(!is_pair_v<T>)
  Any &operator=(std::initializer_list<T> il) {
    Base::operator=(std::vector(il));
    this->m_vtable = std::cref(vtable_ctrl().get<std::vector<T>>());
    this->m_info = Info::init<std::vector<T>>();
    return *this;
  }

  Any &operator=(char const *chars) { return operator=(std::string(chars)); }

  template <class T> bool is() const {
    return this->vtable().type_info() == typeid(T);
  }

  template <class T> T &as() {
    void *ptr = std::any_cast<T>(this);
    gaa_assert(ptr != nullptr, "any cast fails, type {} mismatch",
               typeid(T).name());
    return *reinterpret_cast<T *>(ptr);
  }

  template <class T> T const &as() const {
    void const *ptr = std::any_cast<T>(this);
    gaa_assert(ptr != nullptr, "any cast fails, type {} mismatch",
               typeid(T).name());
    return *reinterpret_cast<T const *>(ptr);
  }

  Info const &info() const;
  Vtable const &vtable() const;
  bool is_null() const;
  std::type_info const &type_info() const;
  std::string format(std::string_view fmt = "{}") const;
  void for_each(Callback callback) const;
  void invoke(std::string const &key, void *input = nullptr,
              void *output = nullptr) const;
};
} // namespace any
using Any = any::Any;
} // namespace gaa