#include <gaa/core/utils.hpp>

namespace gaa {
std::string_view substr(std::string const &str, std::size_t origin,
                        std::size_t length) {
  gaa_assert(str.size() > origin + length);
  if (length == static_cast<std::size_t>(-1)) {
    return std::string_view(str.data() + origin, str.data() + str.size());
  }
  return std::string_view(str.data() + origin, str.data() + origin + length);
}
} // namespace gaa