#include <gaa/core/r_lang.hpp>

#if GAA_WITH_R

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

void read_csv(std::string const &fname, R::DataFrame &df) {}
} // namespace gaa

#endif
