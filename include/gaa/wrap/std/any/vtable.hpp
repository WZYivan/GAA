#pragma once

#include <functional>
#include <map>
#include <string>

#include <gaa/core/keywords.hpp>
#include <gaa/core/literal/def.hpp>
#include <gaa/wrap/std/concepts.hpp>

namespace gaa {
namespace any {

class Any;

struct Vtable {
  /// Plugin gives user full control
  using Plugin =
      std::function<void(Any const &self, void *input, void *output)>;
  using Callback = std::function<void(Any const &self)>;

  std::map<std::string, Plugin> plugins;

  std::function<std::type_info const &(void)> type_info;
  std::function<std::string(void)> type_name;
  std::function<std::string(Any const &self, std::string_view fmt)> format;
  std::function<std::size_t(Any const &self)> size;
  std::function<void(Any const &self, Callback callback)> for_each;
  std::function<void(Any &self, Any const &elem)> push_back;
  std::function<void(Any &self, std::string const &cnt)> push_back_literal;
  std::function<void(Any &self, std::string const &cnt, Literal_Type lt)>
      push_back_literal_2;
  std::function<void(Any &self, std::size_t idx, Any const &elem)> assign_at;

  template <class T> static Vtable init();
};

using Callback = Vtable::Callback;
using Plugin = Vtable::Plugin;
} // namespace any
} // namespace gaa