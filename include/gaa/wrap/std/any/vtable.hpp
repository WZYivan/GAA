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
  using Plugin =
      std::function<void(Any const &any, Any const &args, void *ret)>;

  std::map<std::string, Plugin> plugins;

  std::function<std::type_info const &(void)> type_info;
  std::function<std::string(Any const &any, std::string_view)> format;

  template <class T> static Vtable init() {
    Vtable vtable;
    using D = std::decay_t<T>;

    vtable.type_info = []() -> std::type_info const & { return typeid(D); };
    vtable.format = [](Any const &any, std::string_view fmt) -> std::string {
      if constexpr (std::formattable<D, char>) {
        void const *ptr = std::any_cast<D>(&any);
        if (ptr != nullptr && !is_null(any)) {
          return std::vformat(
              fmt, std::make_format_args(*reinterpret_cast<D const *>(ptr)));
        } else {
          return "Null";
        }
      } else {
        return std::format("unformattable({})", typeid(D).name());
      }
    };

    return vtable;
  }
};
} // namespace any
} // namespace gaa