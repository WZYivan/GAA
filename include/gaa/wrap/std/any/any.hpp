#pragma once

#include <any>
#include <format>

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
    gaa_assert(ptr != nullptr, "any cast fails, given {}, this is {}",
               typeid(T).name(), this->type_info().name());
    return *reinterpret_cast<T *>(ptr);
  }

  template <class T> T const &as() const {
    void const *ptr = std::any_cast<T>(this);
    gaa_assert(ptr != nullptr, "any cast fails, given {}, this is {}",
               typeid(T).name(), this->type_info().name());
    return *reinterpret_cast<T const *>(ptr);
  }

  Info const &info() const;
  Vtable const &vtable() const;
  bool is_null() const;

  std::type_info const &type_info() const;
  std::string format(std::string_view fmt = "{}") const;
  void for_each(Callback callback) const;
  template <class T> void push_back(T &&elem) {
    this->vtable().push_back(*this, std::forward<T>(elem));
  }
  void push_back_literal(std::string const &cnt);
  void push_back_literal(std::string const &cnt, Literal_Type lt);
  std::size_t size() const;

  void invoke(std::string const &key, void *input = nullptr,
              void *output = nullptr) const;
};
} // namespace any

using Any = any::Any;
extern Any literal_cast(std::string const &cnt, Literal_Type type);

namespace any {
template <class T> Vtable Vtable::init() {
  using D = std::decay_t<T>;
  return Vtable{
      .plugins = {},
      .type_info = []() -> std::type_info const & { return typeid(D); },
      .format = [](Any const &self, std::string_view fmt) -> std::string {
        if constexpr (std::formattable<D, char>) {
          void const *ptr = std::any_cast<D>(&self);
          if (ptr != nullptr && !self.is_null()) {
            return std::vformat(
                fmt, std::make_format_args(*reinterpret_cast<D const *>(ptr)));
          } else {
            return "Null";
          }
        } else {
          return std::format("unformattable({})", typeid(D).name());
        }
      },
      .size = [](Any const &self) -> std::size_t {
        if constexpr (Has_Size<D>) {
          return self.as<D>().size();
        } else {
          gaa_fail("{} has no member size", typeid(D).name());
        }
      },
      .for_each = [](Any const &self, Callback callback) -> void {
        if constexpr (std::ranges::range<D>) {
          void const *ptr = std::any_cast<D>(&self);
          if (ptr == nullptr || self.is_null()) {
            return;
          }
          D const *obj = reinterpret_cast<D const *>(ptr);

          auto it = obj->begin();
          auto end = obj->end();

          for (; it != end; ++it) {
            callback(*it);
          }
        } else {
          return callback(self);
        }
      },
      .push_back = [](Any &self, Any const &elem) -> void {
        if constexpr (Has_Push_Back<D>) {
          gaa_assert(elem.type_info() == typeid(typename D::value_type),
                     "push_back argument type mismatch: given {}, expect {}",
                     elem.type_info().name(),
                     typeid(typename D::value_type).name());
          self.as<D>().push_back(elem.template as<typename D::value_type>());
        } else {
          gaa_fail("{} has no member push_back", typeid(D).name());
        }
      },
      .push_back_literal = [](Any &self, std::string const &cnt) -> void {
        if constexpr (Has_Push_Back<D>) {
          self.as<D>().push_back(
              literal_cast(cnt, literal_enum_v<typename D::value_type>)
                  .template as<typename D::value_type>());
        } else {
          gaa_fail("{} has no member push_back", typeid(D).name());
        }
      },
      .push_back_literal_2 = [](Any &self, std::string const &cnt,
                                Literal_Type lt) -> void {
        if constexpr (Has_Push_Back<D>) {
          self.as<D>().push_back(
              literal_cast(cnt, lt).template as<typename D::value_type>());
        } else {
          gaa_fail("{} has no member push_back", typeid(D).name());
        }
      }};
}
} // namespace any
} // namespace gaa