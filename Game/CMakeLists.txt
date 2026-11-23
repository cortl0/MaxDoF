# Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
# https://github/cortl0/MaxDoF
# Licensed under the GNU Affero General Public License v3.0 or later
# See LICENSE file in the project root for full license information

cmake_minimum_required(VERSION 3.16)

project(MaxDoFGame)

if(WIN32)
    set(SDL2_DIR "${MAXDOF_PATH_DEPENDENCIES}/sdl2-windows/x86_64-w64-mingw32/lib/cmake/SDL2")
endif()

find_package(SDL2 REQUIRED)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(OGRE_CFG_SUFFIX "_d")
    set(BUILD_SUBDIR "Debug")
else()
    set(OGRE_CFG_SUFFIX "")
    set(BUILD_SUBDIR "Release")
endif()

set(JOLT_SOURCE "${MAXDOF_PATH_DEPENDENCIES}/jolt-physics" CACHE STRING "Path to Jolt Physics source code" FORCE)
set(JOLT_BINARIES "${MAXDOF_PATH_DEPENDENCIES}/jolt-physics/build/${MAXDOF_TARGET_PLATFORM}/${BUILD_SUBDIR}" CACHE STRING "Path to Jolt Physics build folder" FORCE)

set(OGRE_SOURCE "${MAXDOF_PATH_DEPENDENCIES}/ogre-next" CACHE STRING "Path to OGRE-Next source code" FORCE)
set(OGRE_BINARIES "${MAXDOF_PATH_DEPENDENCIES}/ogre-next/build/${MAXDOF_TARGET_PLATFORM}/${BUILD_SUBDIR}" CACHE STRING "Path to OGRE-Next build folder" FORCE)

set(STBI_SOURCE "${MAXDOF_PATH_DEPENDENCIES}/stbi" CACHE STRING "Path to STBI source code" FORCE)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Templates/MaxDoFWorkspace.compositor"
    "${CMAKE_SOURCE_DIR}/bin/Data/MaxDoFWorkspace.compositor"
    COPYONLY
)

file(MAKE_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}/Data/Textures")
file(COPY "${OGRE_SOURCE}/Samples/Media/Hlms" DESTINATION "${CMAKE_SOURCE_DIR}/bin/Data")
file(COPY "${OGRE_SOURCE}/Samples/Media/2.0" DESTINATION "${CMAKE_SOURCE_DIR}/bin/Data")
file(COPY "${OGRE_SOURCE}/Samples/Media/packs/DebugPack.zip" DESTINATION "${CMAKE_SOURCE_DIR}/bin/Data")

if(WIN32)
    set(OGRE_PLUGIN_DIR ".")
    if(MSVC)
        set(OGRE_PLUGIN_RS_D3D11 "Plugin=RenderSystem_Direct3D11${OGRE_CFG_SUFFIX}")
    else()
        set(OGRE_PLUGIN_RS_D3D11 "# Direct3D11 disabled on MinGW")
    endif()
    set(OGRE_PLUGIN_RS_GL3PLUS "Plugin=RenderSystem_GL3Plus${OGRE_CFG_SUFFIX}")
    set(OGRE_PLUGIN_RS_VULKAN "Plugin=RenderSystem_Vulkan${OGRE_CFG_SUFFIX}")
    set(OGRE_PLUGIN_RS_METAL "# Metal not available on Windows")
else()
    set(OGRE_PLUGIN_DIR "${OGRE_BINARIES}/lib")
    set(OGRE_PLUGIN_RS_D3D11 "# Direct3D11 not available on Linux")
    set(OGRE_PLUGIN_RS_GL3PLUS "Plugin=RenderSystem_GL3Plus${OGRE_CFG_SUFFIX}")
    set(OGRE_PLUGIN_RS_VULKAN "Plugin=RenderSystem_Vulkan${OGRE_CFG_SUFFIX}")
    set(OGRE_PLUGIN_RS_METAL "# Metal not available on Linux")
endif()

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Templates/Plugins.cfg.in"
    "${EXECUTABLE_OUTPUT_PATH}/plugins${OGRE_CFG_SUFFIX}.cfg"
    @ONLY
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Templates/Resources.cfg.in"
    "${EXECUTABLE_OUTPUT_PATH}/resources2.cfg"
    @ONLY
)

file(GLOB_RECURSE SRC_FILES_Game "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE SRC_FILES_Common "${CMAKE_SOURCE_DIR}/Common/*.cpp")

add_executable(${PROJECT_NAME} ${SRC_FILES_Game} ${SRC_FILES_Common})

include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/AudioSystem.cmake")
setup_audio_system(${PROJECT_NAME} ${MAXDOF_PATH_DEPENDENCIES})
add_faust_dsp(${PROJECT_NAME} "Game/Audio/Dsp/SineTest.dsp" "SineTest")

target_compile_definitions(${PROJECT_NAME} PRIVATE
    MAXDOF_CLIENT_BUILD
    JPH_SHARED_LIBRARY
)

target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${JOLT_SOURCE}
    "${OGRE_BINARIES}/include"
    "${OGRE_BINARIES}/include/OGRE"
    "${OGRE_BINARIES}/OgreMain/include"
    "${OGRE_SOURCE}/OgreMain/include"
    "${OGRE_SOURCE}/Components/Hlms/Common/include"
    "${OGRE_SOURCE}/Components/Hlms/Pbs/include"
    "${OGRE_SOURCE}/Components/Hlms/Unlit/include"
    "${OGRE_SOURCE}/Components/Overlay/include"
    "${STBI_SOURCE}"
)

target_link_directories(${PROJECT_NAME} PRIVATE
    "${JOLT_BINARIES}"
    "${JOLT_BINARIES}/bin"
    "${OGRE_BINARIES}/lib"
    "${OGRE_BINARIES}/bin"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Jolt
    "OgreNextMain${OGRE_CFG_SUFFIX}"
    "OgreNextHlmsPbs${OGRE_CFG_SUFFIX}"
    "OgreNextHlmsUnlit${OGRE_CFG_SUFFIX}"
    "OgreNextOverlay${OGRE_CFG_SUFFIX}"
    $<$<TARGET_EXISTS:SDL2::SDL2main>:SDL2::SDL2main>
    SDL2::SDL2
)

if(WIN32)
    set(MINGW_RUNTIME_DLLS "")
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

    file(GLOB OGRE_DLLS "${OGRE_BINARIES}/bin/*.dll")

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:SDL2::SDL2>"
            "${JOLT_BINARIES}/libJolt.dll"
            ${OGRE_DLLS}
            ${MINGW_RUNTIME_DLLS}
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        COMMENT "Copying runtime DLLs for MaxDoFGame"
    )
endif()

if(MINGW)
    target_link_options(${PROJECT_NAME} PRIVATE -mconsole)
endif()
