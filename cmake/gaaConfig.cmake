if(GAA_WITH_R)
    set(cfg_GAA_WITH_R 1)
else()
    set(cfg_GAA_WITH_R 0)
endif()

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.hpp.in
    ${CMAKE_CURRENT_SOURCE_DIR}/include/gaa/core/config.hpp
    @ONLY
)