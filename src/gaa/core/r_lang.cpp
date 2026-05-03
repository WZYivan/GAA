#include "RInside.h"
#include "gaa/core/keywords.hpp"
#include <gaa/core/r_lang.hpp>
#include <memory>

namespace gaa {
void init_r_interp() { r_interp_ptr = std::make_unique<RInside>(); }
void end_r_interp() {
  gaa_assert(r_interp_ptr != nullptr);
  r_interp_ptr.reset();
}
RInside &r_interp() {
  gaa_assert(r_interp_ptr != nullptr);
  return *r_interp_ptr;
}
} // namespace gaa
