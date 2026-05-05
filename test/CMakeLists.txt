if(NOT GAA_USE_PRIVATE_3RD)
    find_package(Boost REQUIRED COMPONENTS unit_test_framework)
endif()

file(GLOB GAA_TEST_SRC *.cpp)

foreach(TEST_SRC ${GAA_TEST_SRC})
    get_filename_component(TEST_SRC_NAME_WE ${TEST_SRC} NAME_WE)
    set(TEST_CASE ${TEST_SRC_NAME_WE})
    add_executable(${TEST_CASE} ${TEST_SRC})
    target_link_libraries(${TEST_CASE} PRIVATE gaa)
    target_compile_definitions(${TEST_CASE} PRIVATE GAA_IN_TEST)
    if(NOT GAA_USE_PRIVATE_3RD)
        target_link_libraries(${TEST_CASE} PRIVATE gaa Boost::unit_test_framework)
    endif()

    add_test(NAME ${TEST_CASE} COMMAND ${TEST_CASE})
endforeach()
