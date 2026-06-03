#pragma once

#define GAA_BUILD_TYPE "Debug"
#define GAA_VERSION "1.0.0"

#if defined(_MSC_VER)
#define GAA_MSVC 1
#else
#define GAA_MSVC 0
#endif

#if defined(__GNUC__)
#define GAA_GNU 1
#else
#define GAA_GNU 0
#endif

#if defined(__clang__)
#define GAA_CLANG 1
#else
#define GAA_CLANG 0
#endif

#define GAA_R_RHOME ""
#define GAA_WITH_R 0
#define GAA_LINK_WITH_CXX_EXP 1
#define GAA_EXT_VIS 1
#define GAA_USE_PRIVATE_3RD 0

#ifndef GAA_AFTER_ASSERTION_FAIL
#define GAA_AFTER_ASSERTION_FAIL std::abort()
#endif

#include <gaa/core/buildsystem/export.hpp>

#if (GAA_MSVC)
#define GAA_msvc_dll_patch GAA_API
#else
#define GAA_msvc_dll_patch
#endif
