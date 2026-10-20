# https://Github/cortl0/MaxDoF
# GPL-3.0 license

cmake_minimum_required( VERSION 3.5.1 )

project(MaxDoF VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(JPH_DOUBLE_PRECISION)

configure_file("${CMAKE_SOURCE_DIR}/CMake/version.h.in" "${CMAKE_BINARY_DIR}/version.h")
include_directories(${CMAKE_BINARY_DIR})

set(MAXDOF_PATH_DEPENDENCIES "${CMAKE_SOURCE_DIR}/../MaxDoF-Dependencies")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}")

add_subdirectory(GameClient)
add_subdirectory(GameServer)
