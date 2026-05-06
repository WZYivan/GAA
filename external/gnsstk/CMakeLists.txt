set(GNSSTK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(GNSSTK_INCLUDE_DIR "${GNSSTK_ROOT}/include")
set(GNSSTK_SRC_DIR "${GNSSTK_ROOT}/src")

file(GLOB GNSSTK_SRC "${GNSSTK_SRC_DIR}/*.cpp" "${GNSSTK_SRC_DIR}/*.c")

add_library(GNSSTK OBJECT ${GNSSTK_SRC})
target_compile_options(GNSSTK PRIVATE -fPIC)
target_include_directories(GNSSTK PUBLIC ${GNSSTK_INCLUDE_DIR})
target_compile_definitions(GNSSTK PRIVATE GNSSTK_EXPORT=)