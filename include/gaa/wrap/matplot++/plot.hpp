#pragma once

#include <gaa/core/buildsystem/config.hpp>

#if (GAA_EXT_VIS)

#include <matplot/matplot.h>

namespace gaa {
namespace plt {
using namespace ::matplot;
}
} // namespace gaa

#else

#if !defined(GAA_IN_BUILDING)
#if (GAA_GNU) || (GAA_CLANG)
#warning                                                                       \
    "GAA is configured without matplot++, it's impossible to visualize, please rebuild GAA if possible"
#elif (GAA_MSVC)
#pragma message(                                                               \
    "GAA is configured without matplot++, it's impossible to visualize, please rebuild GAA if possible")
#endif
#endif

#endif