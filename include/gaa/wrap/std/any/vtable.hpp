#pragma once

#include <any>
#include <format>
#include <functional>
#include <map>
#include <string>

namespace gaa {
namespace any {

class Any;
extern bool is_null(Any const &);

struct Vtable {
  /// Plugin gives user full control
  using Plugin = std::function<void(Any const &any, void *input, void *output)>;
  using Callback = std::function<void(Any const &)>;

  std::map<std::string, Plugin> plugins;

  std::function<std::type_info const &(void)> type_info;
  std::function<std::string(Any const &self, std::string_view)> format;
  std::function<void(Any const &self, Callback callback)> for_each;

  template <class T> static Vtable init() {
    Vtable vtable;
    using D = std::decay_t<T>;

    vtable.type_info = []() -> std::type_info const & { return typeid(D); };

    vtable.format = [](Any const &self, std::string_view fmt) -> std::string {
      if constexpr (std::formattable<D, char>) {
        void const *ptr = std::any_cast<D>(&self);
        if (ptr != nullptr && !is_null(self)) {
          return std::vformat(
              fmt, std::make_format_args(*reinterpret_cast<D const *>(ptr)));
        } else {
          return "Null";
        }
      } else {
        return std::format("unformattable({})", typeid(D).name());
      }
    };

    vtable.for_each = [](Any const &self, Callback callback) {
      if constexpr (std::ranges::range<D>) {
        void const *ptr = std::any_cast<D>(&self);
        if (ptr == nullptr || is_null(self)) {
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
    };

    return vtable;
  }
};

using Callback = Vtable::Callback;
using Plugin = Vtable::Plugin;
} // namespace any
} // namespace gaa