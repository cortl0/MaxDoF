# https://Github/cortl0/MaxDoF
# GPL-3.0 license

cmake_minimum_required( VERSION 3.5.1 )

project(MaxDoF)

set(MAXDOF_PATH_DEPENDENCIES "${CMAKE_SOURCE_DIR}/../MaxDoF-Dependencies")

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}")

include(CMake/Bootstrap.cmake)

include(CMake/Dependencies/OGRE.cmake)

set(OGRE_SOURCE "${MAXDOF_PATH_DEPENDENCIES}/ogre-next" CACHE STRING "Path to OGRE-Next source code")
set(OGRE_BINARIES "${MAXDOF_PATH_DEPENDENCIES}/ogre-next/build/Debug" CACHE STRING "Path to OGRE-Next build folder")

setupOgre(OGRE_SOURCE, OGRE_BINARIES, OGRE_LIBRARIES, FALSE, FALSE, FALSE)

find_package(SDL2 REQUIRED)

add_recursive(./src SOURCES)
add_recursive(./include HEADERS)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/GameClient/*.cpp")
set(SOURCES ${SOURCES} ${SRC_FILES})
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(SOURCES ${SOURCES} ${SRC_FILES})

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
add_subdirectory(${MAXDOF_PATH_DEPENDENCIES}/jolt-physics/Build ${CMAKE_CURRENT_BINARY_DIR}/JoltBuild)
target_compile_definitions(${PROJECT_NAME} PRIVATE MAXDOF_CLIENT_BUILD)
target_include_directories(${PROJECT_NAME} PRIVATE
    ./include
    ./
    ${MAXDOF_PATH_DEPENDENCIES}/jolt-physics
    ${SDL2_INCLUDE_DIRS}
)
target_link_directories(${PROJECT_NAME} PRIVATE ${MAXDOF_PATH_DEPENDENCIES}/jolt-physics/Build)
target_link_libraries(${PROJECT_NAME} PRIVATE Jolt ${OGRE_LIBRARIES} ${SDL2_LIBRARIES})

add_subdirectory(GameServer)
