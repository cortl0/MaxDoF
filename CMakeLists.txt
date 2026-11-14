# Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
# https://github/cortl0/MaxDoF
# Licensed under the GNU Affero General Public License v3.0 or later
# See LICENSE file in the project root for full license information

cmake_minimum_required(VERSION 3.16)

project(MaxDoF VERSION 0.2.0)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(
    JPH_DOUBLE_PRECISION
    JPH_OBJECT_STREAM
    JPH_USE_SSE4_1
    JPH_USE_SSE4_2
    JPH_USE_AVX
    JPH_USE_AVX2
    JPH_USE_LZCNT
    JPH_USE_TZCNT
    JPH_USE_F16C
    JPH_USE_FMADD
    JPH_USE_CPU_COMPUTE
    JPH_USE_VK
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(
        JPH_DEBUG_RENDERER
        JPH_PROFILE_ENABLED
    )
endif()

configure_file("${CMAKE_SOURCE_DIR}/CMake/version.h.in" "${CMAKE_BINARY_DIR}/version.h")
include_directories(${CMAKE_BINARY_DIR})

set(MAXDOF_PATH_DEPENDENCIES "${CMAKE_SOURCE_DIR}/../MaxDoF-Dependencies")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}")

if(MSVC)
    add_compile_options(/W3 /WX /arch:AVX2)
else()
    add_compile_options(
        -msse4.1
        -msse4.2
        -mavx
        -mavx2
        -mfma
        -mf16c
        -mlzcnt
        -mbmi
        -Werror=all
    )
endif()

add_subdirectory(Game)
add_subdirectory(Server)
