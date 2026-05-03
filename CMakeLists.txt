cmake_minimum_required(VERSION 3.20)

project(GAA LANGUAGES C CXX VERSION 1.0.0)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(GAA_DEV ON)
option(GAA_TEST OFF)
option(GAA_MODULE OFF)
option(GAA_WITH_R ON)
option(GAA_STRICT ON)
option(GAA_VERY_STRICT OFF)


find_package(Boost REQUIRED COMPONENTS headers)
find_package(Eigen3 REQUIRED NO_MODULE)
if(GAA_WITH_R)
    include(gaaFindRLang)
    include(gaaFindRPackage)
    gaaFindRPackage(Rcpp)
    gaaFindRPackage(RInside)

    if(NOT Rcpp_FOUND)
        message(FATAL_ERROR "R package Rcpp is REQUIRED but not found")
    endif()

    if(NOT RInside_FOUND)
        message(FATAL_ERROR "R package RInside is REQUIRED but not found")
    endif()
endif()

set(GAA_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(GAA_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(GAA_DEV_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dev)

include(gaaConfig)

add_subdirectory(src)

include(gaaInstall)
include(gaaUninstall)
include(gaaCMakeConfig)

if(GAA_DEV)
    add_subdirectory(dev)
endif()

if(GAA_MODULE)
    add_subdirectory(module)
endif()

if(GAA_TEST)
    enable_testing()
    add_subdirectory(test)
endif()