#pragma once

#include <gaa/core/config.hpp>
#include <gaa/geodesy/ellipsoid/fwd.hpp>

namespace gaa {
extern GAA_msvc_dll_patch Ellipsoid const cgcs2000;
extern GAA_msvc_dll_patch Ellipsoid const wgs84;
extern GAA_msvc_dll_patch Ellipsoid const krassovsky;
extern GAA_msvc_dll_patch Ellipsoid const null_ellipsoid;
} // namespace gaa