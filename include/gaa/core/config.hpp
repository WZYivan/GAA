#pragma once

#define GAA_BUILD_TYPE "Debug"
#define GAA_VERSION "1.0.0"

#define GAA_R_RHOME "/usr/local/lib/R"
#define GAA_WITH_R 1

#ifndef GAA_AFTER_ASSERTION_FAIL
#define GAA_AFTER_ASSERTION_FAIL std::abort
#endif
