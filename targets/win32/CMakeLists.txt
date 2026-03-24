#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

cmake_minimum_required(VERSION 3.25)

# BUILD_VERSION can be passed from CI; default to dev sentinel.
if(NOT DEFINED BUILD_VERSION)
    set(BUILD_VERSION "9.99.999.9999")
endif()

# Break BUILD_VERSION into components for VERSION_MAJOR/MINOR/BUILD/REVISION.
string(REPLACE "." ";" _VERSION_PARTS "${BUILD_VERSION}")
list(LENGTH _VERSION_PARTS _VERSION_PARTS_LEN)
if(_VERSION_PARTS_LEN GREATER_EQUAL 1)
    list(GET _VERSION_PARTS 0 NBGV_VERSION_MAJOR)
else()
    set(NBGV_VERSION_MAJOR 9)
endif()
if(_VERSION_PARTS_LEN GREATER_EQUAL 2)
    list(GET _VERSION_PARTS 1 NBGV_VERSION_MINOR)
else()
    set(NBGV_VERSION_MINOR 99)
endif()
if(_VERSION_PARTS_LEN GREATER_EQUAL 3)
    list(GET _VERSION_PARTS 2 NBGV_BUILD_NUMBER)
else()
    set(NBGV_BUILD_NUMBER 999)
endif()
if(_VERSION_PARTS_LEN GREATER_EQUAL 4)
    list(GET _VERSION_PARTS 3 TARGET_BUILD_COUNTER)
else()
    set(TARGET_BUILD_COUNTER 9999)
endif()

project(nanoFramework_nanoCLR_win32
    VERSION "${NBGV_VERSION_MAJOR}.${NBGV_VERSION_MINOR}.${NBGV_BUILD_NUMBER}"
    LANGUAGES C CXX
)

if(NOT WIN32)
    message(FATAL_ERROR "targets/win32 can only be built on Windows.")
endif()

add_subdirectory(nanoCLR)
