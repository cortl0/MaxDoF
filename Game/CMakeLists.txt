# Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
# https://github/cortl0/MaxDoF
# Licensed under the GNU Affero General Public License v3.0 or later
# See LICENSE file in the project root for full license information

cmake_minimum_required(VERSION 3.16)

project(MaxDoFGame)

find_package(SDL2 REQUIRED)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(OGRE_CFG_SUFFIX "_d")
    set(OGRE_BUILD_SUBDIR "Debug")
else()
    set(OGRE_CFG_SUFFIX "")
    set(OGRE_BUILD_SUBDIR "Release")
endif()

set(OGRE_SOURCE "${MAXDOF_PATH_DEPENDENCIES}/ogre-next" CACHE STRING "Path to OGRE-Next source code" FORCE)
set(OGRE_BINARIES "${MAXDOF_PATH_DEPENDENCIES}/ogre-next/build/${OGRE_BUILD_SUBDIR}" CACHE STRING "Path to OGRE-Next build folder" FORCE)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Templates/MaxDoFWorkspace.compositor"
    "${CMAKE_SOURCE_DIR}/bin/Data/MaxDoFWorkspace.compositor"
    COPYONLY
)
file(COPY "${OGRE_SOURCE}/Samples/Media/Hlms" DESTINATION "${CMAKE_SOURCE_DIR}/bin/Data")
file(COPY "${OGRE_SOURCE}/Samples/Media/2.0" DESTINATION "${CMAKE_SOURCE_DIR}/bin/Data")
file(COPY "${OGRE_SOURCE}/Samples/Media/packs/DebugPack.zip" DESTINATION "${CMAKE_SOURCE_DIR}/bin/Data")

if(WIN32)
    set(OGRE_PLUGIN_DIR "${OGRE_BINARIES}/bin")
    set(OGRE_PLUGIN_RS_D3D11 "Plugin=RenderSystem_Direct3D11${OGRE_CFG_SUFFIX}")
    set(OGRE_PLUGIN_RS_GL3PLUS "Plugin=RenderSystem_GL3Plus${OGRE_CFG_SUFFIX}")
    set(OGRE_PLUGIN_RS_VULKAN "Plugin=RenderSystem_Vulkan${OGRE_CFG_SUFFIX}")
    set(OGRE_PLUGIN_RS_METAL "# Metal not available on Windows")
elseif(APPLE)
    set(OGRE_PLUGIN_DIR "${OGRE_BINARIES}/lib")
    set(OGRE_PLUGIN_RS_D3D11 "# Direct3D11 not available on macOS")
    set(OGRE_PLUGIN_RS_GL3PLUS "# GL3Plus deprecated on macOS")
    set(OGRE_PLUGIN_RS_VULKAN "Plugin=RenderSystem_Vulkan${OGRE_CFG_SUFFIX}")
    set(OGRE_PLUGIN_RS_METAL "Plugin=RenderSystem_Metal${OGRE_CFG_SUFFIX}")
else()
    set(OGRE_PLUGIN_DIR "${OGRE_BINARIES}/lib")
    set(OGRE_PLUGIN_RS_D3D11 "# Direct3D11 not available on Linux")
    set(OGRE_PLUGIN_RS_GL3PLUS "Plugin=RenderSystem_GL3Plus${OGRE_CFG_SUFFIX}")
    set(OGRE_PLUGIN_RS_VULKAN "Plugin=RenderSystem_Vulkan${OGRE_CFG_SUFFIX}")
    set(OGRE_PLUGIN_RS_METAL "# Metal not available on Linux")
endif()

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Templates/Plugins.cfg.in"
    "${CMAKE_SOURCE_DIR}/bin/Data/plugins${OGRE_CFG_SUFFIX}.cfg"
    @ONLY
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Templates/Resources.cfg.in"
    "${CMAKE_SOURCE_DIR}/bin/Data/resources2.cfg"
    @ONLY
)

file(GLOB_RECURSE SRC_FILES_Game "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE SRC_FILES_Common "${CMAKE_SOURCE_DIR}/Common/*.cpp")

if(APPLE)
    set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/PlatformPaths.cpp" PROPERTIES COMPILE_FLAGS "-x objective-c++")
endif()

add_executable(${PROJECT_NAME} ${SRC_FILES_Game} ${SRC_FILES_Common})
add_subdirectory(${MAXDOF_PATH_DEPENDENCIES}/jolt-physics/Build ${CMAKE_CURRENT_BINARY_DIR}/JoltBuild)
target_compile_definitions(${PROJECT_NAME} PRIVATE MAXDOF_CLIENT_BUILD)

target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${MAXDOF_PATH_DEPENDENCIES}/jolt-physics
    ${SDL2_INCLUDE_DIRS}
    "${OGRE_BINARIES}/include"
    "${OGRE_BINARIES}/include/OGRE"
    "${OGRE_BINARIES}/OgreMain/include"
    "${OGRE_SOURCE}/OgreMain/include"
    "${OGRE_SOURCE}/Components/Hlms/Common/include"
    "${OGRE_SOURCE}/Components/Hlms/Pbs/include"
    "${OGRE_SOURCE}/Components/Hlms/Unlit/include"
    "${OGRE_SOURCE}/Components/Overlay/include"
)

target_link_directories(${PROJECT_NAME} PRIVATE
    "${MAXDOF_PATH_DEPENDENCIES}/jolt-physics/Build"
    "${OGRE_BINARIES}/lib"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Jolt
    "OgreNextMain${OGRE_CFG_SUFFIX}"
    "OgreNextHlmsPbs${OGRE_CFG_SUFFIX}"
    "OgreNextHlmsUnlit${OGRE_CFG_SUFFIX}"
    "OgreNextOverlay${OGRE_CFG_SUFFIX}"
    ${SDL2_LIBRARIES}
)
