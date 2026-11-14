# Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
# https://github/cortl0/MaxDoF
# Licensed under the GNU Affero General Public License v3.0 or later
# See LICENSE file in the project root for full license information

cmake_minimum_required( VERSION 3.5.1 )

project(MaxDoFGame)

find_package(SDL2 REQUIRED)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/CMake/Templates/MaxDoFWorkspace.compositor" "${CMAKE_SOURCE_DIR}/bin/Data/MaxDoFWorkspace.compositor")

include(CMake/Bootstrap.cmake)
include(CMake/Dependencies/OGRE.cmake)

set(OGRE_SOURCE "${MAXDOF_PATH_DEPENDENCIES}/ogre-next" CACHE STRING "Path to OGRE-Next source code")
set(OGRE_BINARIES "${MAXDOF_PATH_DEPENDENCIES}/ogre-next/build/Debug" CACHE STRING "Path to OGRE-Next build folder")

setupOgre(OGRE_SOURCE, OGRE_BINARIES, OGRE_LIBRARIES, TRUE, FALSE, FALSE)

file(COPY "${OGRE_SOURCE}/Samples/Media/Hlms" DESTINATION "${CMAKE_SOURCE_DIR}/bin/Data")
file(COPY "${OGRE_SOURCE}/Samples/Media/2.0" DESTINATION "${CMAKE_SOURCE_DIR}/bin/Data")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(OGRE_CFG_SUFFIX "_d")
else()
    set(OGRE_CFG_SUFFIX "")
endif()

# Задаём плагины под текущую платформу
if(WIN32)
    set(OGRE_PLUGIN_DIR "${OGRE_BINARIES}/bin")
    set(OGRE_PLUGIN_RS_D3D11 "Plugin=RenderSystem_Direct3D11${OGRE_CFG_SUFFIX}")
    set(OGRE_PLUGIN_RS_GL3PLUS "Plugin=RenderSystem_GL3Plus${OGRE_CFG_SUFFIX}")
    set(OGRE_PLUGIN_RS_VULKAN "Plugin=RenderSystem_Vulkan${OGRE_CFG_SUFFIX}")
    set(OGRE_PLUGIN_RS_METAL "# Metal not available on Windows")
elseif(APPLE)
    # На macOS при разработке плагины лежат в lib/, а при сборке .app бандла — в Contents/Plugins
    set(OGRE_PLUGIN_DIR "${OGRE_BINARIES}/lib")
    set(OGRE_PLUGIN_RS_D3D11 "# Direct3D11 not available on macOS")
    set(OGRE_PLUGIN_RS_GL3PLUS "# GL3Plus deprecated on macOS")
    set(OGRE_PLUGIN_RS_VULKAN "Plugin=RenderSystem_Vulkan${OGRE_CFG_SUFFIX}")
    set(OGRE_PLUGIN_RS_METAL "Plugin=RenderSystem_Metal${OGRE_CFG_SUFFIX}")
else()
    # Linux
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

# Копируем и заполняем шаблон в папку bin/Data с нужным именем
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Templates/Plugins.cfg.in"
    "${CMAKE_SOURCE_DIR}/bin/Data/plugins${OGRE_CFG_SUFFIX}.cfg"
    @ONLY
)

file(GLOB_RECURSE SOURCES_OGRE "${CMAKE_CURRENT_SOURCE_DIR}/OgreCommon/src")
file(GLOB_RECURSE SRC_FILES_Game "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE SRC_FILES_Common "${CMAKE_SOURCE_DIR}/Common/*.cpp")

file(GLOB_RECURSE SRC_FILES_Game "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
list(FILTER SRC_FILES_Game EXCLUDE REGEX "${CMAKE_CURRENT_SOURCE_DIR}/OgreCommon/.*")

file(GLOB_RECURSE SRC_FILES_Common "${CMAKE_SOURCE_DIR}/Common/*.cpp")

add_executable(${PROJECT_NAME} ${SRC_FILES_Game} ${SRC_FILES_Common})



# add_executable(${PROJECT_NAME} ${SOURCES_OGRE} ${SRC_FILES_Game} ${SRC_FILES_Common})
add_subdirectory(${MAXDOF_PATH_DEPENDENCIES}/jolt-physics/Build ${CMAKE_CURRENT_BINARY_DIR}/JoltBuild)
target_compile_definitions(${PROJECT_NAME} PRIVATE MAXDOF_CLIENT_BUILD)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${MAXDOF_PATH_DEPENDENCIES}/jolt-physics
    ${SDL2_INCLUDE_DIRS}
)
target_link_directories(${PROJECT_NAME} PRIVATE ${MAXDOF_PATH_DEPENDENCIES}/jolt-physics/Build)
target_link_libraries(${PROJECT_NAME} PRIVATE Jolt ${OGRE_LIBRARIES} ${SDL2_LIBRARIES})
