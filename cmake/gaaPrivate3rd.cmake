set(GAA_3RD_DIR "${GAA_ROOT}/3rd/")
set(GAA_3RD_INCLUDE_DIR "${GAA_3RD_DIR}/include")
set(GAA_3RD_LIB_DIR "${GAA_3RD_DIR}/lib")

target_include_directories(gaa PUBLIC $<BUILD_INTERFACE:${GAA_3RD_INCLUDE_DIR}>)
target_include_directories(gaa PUBLIC $<BUILD_INTERFACE:${GAA_3RD_INCLUDE_DIR}/eigen3>)

target_link_directories(gaa PUBLIC $<BUILD_INTERFACE:${GAA_3RD_LIB_DIR}>)