file(GLOB_RECURSE GAA_SRC_FILES ${GAA_SRC_DIR}/**/*.cpp)

add_library(gaa SHARED ${GAA_SRC_FILES})
target_include_directories(gaa PUBLIC $<BUILD_INTERFACE:${GAA_INCLUDE_DIR}>)
target_compile_definitions(gaa PRIVATE GAA_IN_BUILDING)

if(GAA_USE_PRIVATE_3RD)
    include(gaaPrivate3rd)
else()
    target_link_libraries(gaa 
    PUBLIC Boost::headers Eigen3::Eigen DataFrame::DataFrame tbb)
endif()

if(NOT GAA_USE_PRIVATE_EXTERNAL)
    target_link_libraries(gaa PUBLIC gnsstk)
elseif(GAA_USE_GNSSTK)
    target_sources(gaa PRIVATE $<TARGET_OBJECTS:GNSSTK>)
    target_include_directories(gaa PRIVATE ${GNSSTK_INCLUDE_DIR})
endif()

if(GAA_WITH_R)
    target_include_directories(gaa PUBLIC ${Rcpp_INCLUDE_DIR} ${RInside_INCLUDE_DIR})
    target_compile_options(gaa PUBLIC ${R_CPPFLAGS})
    target_link_libraries(gaa PUBLIC ${R_LDFLAGS} ${Rcpp_LDFLAGS} ${RInside_LDFLAGS})
endif()


if(MSVC)
    target_compile_options(gaa PUBLIC 
        /Zc:preprocessor
        /Zc:__cplusplus)
    target_compile_options(gaa PUBLIC /wd4819)
else()
    if(GAA_STRICT)
    target_compile_options(gaa PRIVATE 
        -Wall -Wextra -pedantic -Werror)
    endif()

    if(GAA_VERY_STRICT)
    target_compile_options(gaa PRIVATE 
        -Wall -Wextra -pedantic -Werror
        -Wconversion -Wsign-conversion -Wshadow -Wcast-align
        -Wunused -Wnull-dereference -Wdouble-promotion
        -Wformat=2 -Wimplicit-fallthrough -Wmissing-include-dirs
        -Wold-style-cast -Woverloaded-virtual -Wzero-as-null-pointer-constant
        $<$<CXX_COMPILER_ID:Clang>:-Wdocumentation -Wcomma -Wheader-hygiene>
        )
    endif()
endif()

