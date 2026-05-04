include(xrepo)

function(gaaFindPackageOrXrepo CMAKE_PACKAGE XREPO_PACKAGE)
    find_package(${CMAKE_PACKAGE} QUIET ${ARGN})

    if(NOT ${CMAKE_PACKAGE}_FOUND)
        message(STATUS "${CMAKE_PACKAGE} not found, installing ${XREPO_PACKAGE} via xrepo...")
        xrepo_package(${XREPO_CMAKE})
        find_package(${CMAKE_PACKAGE} REQUIRED ${ARGN})
    endif()
endfunction()
