# Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
# https://github/cortl0/MaxDoF
# Licensed under the GNU Affero General Public License v3.0 or later
# See LICENSE file in the project root for full license information

cmake_minimum_required(VERSION 3.16)

project(MaxDoFServer)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BUILD_SUBDIR "Debug")
else()
    set(BUILD_SUBDIR "Release")
endif()

set(JOLT_SOURCE "${MAXDOF_PATH_DEPENDENCIES}/jolt-physics" CACHE STRING "Path to Jolt Physics source code" FORCE)
set(JOLT_BINARIES "${MAXDOF_PATH_DEPENDENCIES}/jolt-physics/build/${MAXDOF_TARGET_PLATFORM}/${BUILD_SUBDIR}" CACHE STRING "Path to Jolt Physics build folder" FORCE)

file(GLOB_RECURSE SRC_FILES_Server "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE SRC_FILES_Common "${CMAKE_SOURCE_DIR}/Common/*.cpp")

add_executable(${PROJECT_NAME} ${SRC_FILES_Server} ${SRC_FILES_Common})

target_compile_definitions(${PROJECT_NAME} PRIVATE
    JPH_SHARED_LIBRARY
)

target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${JOLT_SOURCE}
)

target_link_directories(${PROJECT_NAME} PRIVATE
    "${JOLT_BINARIES}"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Jolt
)

if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${JOLT_BINARIES}/libJolt.dll"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        COMMENT "Copying Jolt DLL for Server"
    )
endif()

if(MINGW)
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libstdc++-6.dll OUTPUT_VARIABLE MINGW_LIBSTDCPP OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libgcc_s_seh-1.dll OUTPUT_VARIABLE MINGW_LIBGCC OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libwinpthread-1.dll OUTPUT_VARIABLE MINGW_WINPTHREAD OUTPUT_STRIP_TRAILING_WHITESPACE)

    set(MINGW_RUNTIME_DLLS
        "${MINGW_LIBSTDCPP}"
        "${MINGW_LIBGCC}"
        "${MINGW_WINPTHREAD}"
    )
endif()
