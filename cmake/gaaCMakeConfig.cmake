include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/gaaConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/gaaConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/gaaConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/gaa
    PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR
)

export(EXPORT gaaTargets
    NAMESPACE gaa::
    FILE ${CMAKE_CURRENT_BINARY_DIR}/gaaTargets.cmake)
install(EXPORT gaaTargets
    NAMESPACE gaa::
    FILE gaaTargets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/gaa)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/gaaConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/gaaConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/gaa)