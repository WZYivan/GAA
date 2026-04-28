# - Find R package
#
# This module checks that the R package installation.
#
# It defines the following variables:
#   <NAME>_FOUND       - TRUE if the Rcpp package is available
#   <NAME>_ROOT        - installation directory of Rcpp
#   <NAME>_INCLUDE_DIR - path to Rcpp's include/ directory (convenience) * optional
#   <NAME>_LDFLAGS - path to Rcpp's lib/ directory (convenience) * optional
#
# Example usage:
#   include(gaaFindPackage)
#   gaaFindPackage(Rcpp)
#   target_include_directories(my_target PRIVATE ${Rcpp_INCLUDE_DIR})

function(gaaFindRPackage R_PACKAGE_NAME)
    if(NOT R_EXECUTABLE)
        message(FATAL_ERROR "R not found. Cannot locate package ${R_PACKAGE_NAME}")
    endif()

    execute_process(
        COMMAND ${R_EXECUTABLE} --slave --no-restore -e
                "cat(find.package(\"${R_PACKAGE_NAME}\", quiet=TRUE), \"\\n\")"
        OUTPUT_VARIABLE _pkg_root
        ERROR_QUIET
        RESULT_VARIABLE _exit_code
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if(NOT _exit_code EQUAL 0)
        message(FATAL_ERROR "R command failed while searching for package ${R_PACKAGE_NAME} (exit ${_exit_code})")
    endif()

    if(_pkg_root STREQUAL "")
        message(STATUS "R package ${R_PACKAGE_NAME} not found")
        set(${R_PACKAGE_NAME}_FOUND FALSE PARENT_SCOPE)
        return()
    endif()

    message(STATUS "Found R package ${R_PACKAGE_NAME}: ${_pkg_root}")
    set(${R_PACKAGE_NAME}_FOUND TRUE PARENT_SCOPE)
    set(${R_PACKAGE_NAME}_ROOT "${_pkg_root}" PARENT_SCOPE)

    set(_include_dir "${_pkg_root}/include")
    if(EXISTS "${_include_dir}")
        set(${R_PACKAGE_NAME}_INCLUDE_DIR "${_include_dir}" PARENT_SCOPE)
        message(STATUS "R package ${R_PACKAGE_NAME} include dir: ${_include_dir}")
    else()
        set(${R_PACKAGE_NAME}_INCLUDE_DIR "" PARENT_SCOPE)
    endif()

    set(_libdir "${_pkg_root}/lib")
    set(_lib_name_so "lib${R_PACKAGE_NAME}.so")   
    set(_lib_name_a "lib${R_PACKAGE_NAME}.a")

    if(EXISTS "${_libdir}")
        set(${R_PACKAGE_NAME}_LIB_DIR "${_libdir}")
        message(STATUS "R package ${R_PACKAGE_NAME} lib dir: ${_libdir}")

        if(EXISTS "${_libdir}/${_lib_name_so}")
            set(_lib_name "${_lib_name_so}")
        elseif(EXISTS "${_libdir}/${_lib_name_a}")
            set(_lib_name "${_lib_name_a}")
        else()
            message(FATAL_ERROR "Library lib${R_PACKAGE_NAME}.so or .a not found in ${_libdir}")
        endif()

        message(STATUS "Found R package ${R_PACKAGE_NAME} lib: ${_lib_name}")

        set(${R_PACKAGE_NAME}_LDFLAGS "-L${_libdir} -Wl,-rpath=${_libdir} -l:${_lib_name}" PARENT_SCOPE)        
    endif()

    

endfunction()

