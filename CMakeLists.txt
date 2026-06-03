cmake_minimum_required(VERSION 3.20)

if(WIN32 OR MSVC)
    message(WARNING "FUCK MICROSOFT AND ITS STUPID WINDOWS, FUCK YOU ALL!!! AND GO FUCK YOURSELF...")
endif()

project(GAA LANGUAGES C CXX VERSION 1.0.0)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(GAA_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(GAA_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(GAA_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(GAA_DEV_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dev)
set(GAA_EXPORT_MACRO_NAME "GAA_API")

option(GAA_EXT_VIS "Enable gaa/visualization" OFF)
option(GAA_DEV "Building sources in folder dev/" OFF)
option(GAA_TEST "Enable ctest and build sources in test/" ON)
option(GAA_MODULE "(experimental) Building modules" OFF)
option(GAA_DOCS "Building corresponding document (need TeX Live)" ON)
option(GAA_WITH_R "Enable embedding R (need Rcpp and RInside)" OFF)
option(GAA_R_EXT "(experimental) Building Rcpp extension" OFF)
option(GAA_STRICT "Enable serval compiler flags to give more warnings and error" ON)
option(GAA_VERY_STRICT "Enable more compiler flags to give more warnings and error" OFF)
option(GAA_USE_PRIVATE_3RD "Enable build and install using bundle 3rd party" OFF)
option(SYS_WITH_XREPO "Enable install required packages using xrepo (need xmake)" OFF)

if(NOT GAA_USE_PRIVATE_3RD)
    if(WITH_XREPO)
        message(WARNING "xrepo is not strictly test in this project...")
        include(gaaXrepoCMake)
        gaaFindPackageOrXrepo(Boost  "boost" COMPONENTS headers)
        gaaFindPackageOrXrepo(Eigen3 "eigen" NO_MODULE)
    else()
        find_package(Boost REQUIRED COMPONENTS headers)
        find_package(Eigen3 REQUIRED NO_MODULE)
    endif()
elseif(NOT WIN32)
    message(WARNING "bundle boost and Eigen are provided for windows user, it may populate environment (/usr/local/include) in GNU/Linux")
endif()

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

if(GAA_EXT_VIS)
    find_package(Matplot++ REQUIRED)
endif()

include(gaaLinkExpCXX)

add_subdirectory(src)

include(gaaConfig)
include(gaaExport)
include(gaaInstall)
include(gaaUninstall)
include(gaaCMakeConfig)

if(GAA_R_EXT)
    add_subdirectory(R)
endif()

if(GAA_DOCS)
    add_subdirectory(docs)
endif()

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