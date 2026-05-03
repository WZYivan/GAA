file(GLOB_RECURSE GAA_DEV_FILES ${GAA_DEV_DIR}/**/*.cpp)

foreach(GAA_DEV_FILE ${GAA_DEV_FILES})
    get_filename_component(base_name ${GAA_DEV_FILE} NAME_WE)
    set(exe_name "${base_name}")
    add_executable(${exe_name} ${GAA_DEV_FILE})
    target_link_libraries(${exe_name} PRIVATE gaa)
    # target_compile_options(${exe_name} BEFORE PRIVATE -save-temps)
endforeach()


