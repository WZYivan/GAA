# - Find R language
#
# This module looks for the R executable, retrieves its compilation flags
#
# It defines the following variables:
#   R_FOUND          - TRUE if R was found
#   R_EXECUTABLE     - path to the R executable
#   R_CPPFLAGS       - flags from 'R CMD config --cppflags'
#   R_LDFLAGS        - flags from 'R CMD config --ldflags'
#
# Example usage:
#   include(gaaFindRLang)
#   target_compile_options(my_target PRIVATE ${R_CPPFLAGS})
#   target_link_libraries(my_target PRIVATE ${R_LDFLAGS})

find_program(R_EXECUTABLE NAMES R)
if(NOT R_EXECUTABLE)
    message(FATAL_ERROR "R not found. Try: sudo apt install r-base-dev")
endif()
message(STATUS "Found R: ${R_EXECUTABLE}")

execute_process(
    COMMAND ${R_EXECUTABLE} CMD config --cppflags
    OUTPUT_VARIABLE R_CPPFLAGS
    ERROR_QUIET
    RESULT_VARIABLE gaa_R_cppflags_ok
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT gaa_R_cppflags_ok EQUAL 0)
    message(FATAL_ERROR "Error executing 'R CMD config --cppflags'")
endif()
message(STATUS "R C++ flags: ${R_CPPFLAGS}")

execute_process(
    COMMAND ${R_EXECUTABLE} CMD config --ldflags
    OUTPUT_VARIABLE R_LDFLAGS
    ERROR_QUIET
    RESULT_VARIABLE gaa_R_ldflags_ok
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT gaa_R_ldflags_ok EQUAL 0)
    message(FATAL_ERROR "Error executing 'R CMD config --ldflags'")
endif()

execute_process(
    COMMAND ${R_EXECUTABLE} RHOME
    OUTPUT_VARIABLE R_RHOME
    ERROR_QUIET
    RESULT_VARIABLE gaa_R_rhome_ok
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT gaa_R_rhome_ok EQUAL 0)
    message(FATAL_ERROR "Error executing 'R RHOME'")
endif()
set(R_LDFLAGS "${R_LDFLAGS} -Wl,-rpath=${R_RHOME}/lib")
message(STATUS "R linker flags: ${R_LDFLAGS}")

set(R_FOUND TRUE)