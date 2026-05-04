set(XREPO_CMAKE "${CMAKE_CURRENT_BINARY_DIR}/xrepo.cmake")
if(NOT EXISTS ${XREPO_CMAKE})
    message(STATUS "Downloading xrepo.cmake => ${XREPO_CMAKE}")
    file(DOWNLOAD "https://raw.githubusercontent.com/xmake-io/xrepo-cmake/main/xrepo.cmake"
                  "${XREPO_CMAKE}" TLS_VERIFY ON)
endif()
include("${XREPO_CMAKE}")

function(gaaFindPackageOrXrepo CMAKE_PACKAGE XREPO_PACKAGE)
    find_package(${CMAKE_PACKAGE} QUIET ${ARGN})

    if(NOT ${CMAKE_PACKAGE}_FOUND)
        message(STATUS "${CMAKE_PACKAGE} not found, installing ${XREPO_PACKAGE} via xrepo...")
        xrepo_package(${XREPO_CMAKE})
        find_package(${CMAKE_PACKAGE} REQUIRED ${ARGN})
    endif()
endfunction()
