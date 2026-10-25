# https://Github/cortl0/MaxDoF
# GPL-3.0 license

cmake_minimum_required( VERSION 3.5.1 )

project(MaxDoFGame)

find_package(SDL2 REQUIRED)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/CMake/Templates/MaxDoFWorkspace.compositor" "${CMAKE_SOURCE_DIR}/bin/Data/MaxDoFWorkspace.compositor")

include(CMake/Bootstrap.cmake)
include(CMake/Dependencies/OGRE.cmake)

set(OGRE_SOURCE "${MAXDOF_PATH_DEPENDENCIES}/ogre-next" CACHE STRING "Path to OGRE-Next source code")
set(OGRE_BINARIES "${MAXDOF_PATH_DEPENDENCIES}/ogre-next/build/Debug" CACHE STRING "Path to OGRE-Next build folder")

setupOgre(OGRE_SOURCE, OGRE_BINARIES, OGRE_LIBRARIES, FALSE, FALSE, FALSE)

file(GLOB_RECURSE SOURCES_OGRE "${CMAKE_CURRENT_SOURCE_DIR}/OgreCommon/src")
file(GLOB_RECURSE SRC_FILES_GAMECLIENT "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE SRC_FILES_GAMECOMMON "${CMAKE_SOURCE_DIR}/GameCommon/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES_OGRE} ${SRC_FILES_GAMECLIENT} ${SRC_FILES_GAMECOMMON})
add_subdirectory(${MAXDOF_PATH_DEPENDENCIES}/jolt-physics/Build ${CMAKE_CURRENT_BINARY_DIR}/JoltBuild)
target_compile_definitions(${PROJECT_NAME} PRIVATE MAXDOF_CLIENT_BUILD)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${MAXDOF_PATH_DEPENDENCIES}/jolt-physics
    ${SDL2_INCLUDE_DIRS}
)
target_link_directories(${PROJECT_NAME} PRIVATE ${MAXDOF_PATH_DEPENDENCIES}/jolt-physics/Build)
target_link_libraries(${PROJECT_NAME} PRIVATE Jolt ${OGRE_LIBRARIES} ${SDL2_LIBRARIES})
