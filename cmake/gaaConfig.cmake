if(GAA_WITH_R)
    set(IS_GAA_WITH_R 1)
else()
    set(IS_GAA_WITH_R 0)
endif()

set(GAA_BUILD_TYPE ${CMAKE_BUILD_TYPE})

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.hpp.in
    ${CMAKE_CURRENT_SOURCE_DIR}/include/gaa/core/config.hpp
    @ONLY
)