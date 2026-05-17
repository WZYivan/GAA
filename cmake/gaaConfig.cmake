if(GAA_WITH_R)
    set(IS_GAA_WITH_R 1)
else()
    set(IS_GAA_WITH_R 0)
endif()

if(GAA_USE_PRIVATE_3RD)
    set(IS_GAA_USE_PRIVATE_3RD 1)
else()
    set(IS_GAA_USE_PRIVATE_3RD 0)
endif()

if(GAA_EXT_VIS)
    set(IS_GAA_EXT_VIS 1)
else()
    set(IS_GAA_EXT_VIS 0)
endif()

set(GAA_BUILD_TYPE ${CMAKE_BUILD_TYPE})

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.hpp.in
    ${CMAKE_CURRENT_SOURCE_DIR}/include/gaa/core/config.hpp
    @ONLY
)