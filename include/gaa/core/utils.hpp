#pragma once

#include <charconv>
#include <string>
#include <string_view>

#include <gaa/core/keywords.hpp>

namespace gaa {
extern std::string_view substr(std::string const &str, std::size_t origin,
                               std::size_t length = -1);

template <class V> bool from_str(std::string const &str, V &val) {
  auto [ptr, ec] = (std::from_chars(str.data(), str.data() + str.size(), val));
  return ec == std::errc{};
}
} // namespace gaa