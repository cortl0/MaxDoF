# https://Github/cortl0/MaxDoF
# GPL-3.0 license

cmake_minimum_required( VERSION 3.5.1 )
project(MaxDoFServer)

file(GLOB_RECURSE SRC_FILES_GAMESERVER "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE SRC_FILES_GAMECOMMON "${CMAKE_SOURCE_DIR}/GameCommon/*.cpp")

add_executable(${PROJECT_NAME} ${SRC_FILES_GAMESERVER} ${SRC_FILES_GAMECOMMON})
target_link_directories(${PROJECT_NAME} PRIVATE ${MAXDOF_PATH_DEPENDENCIES}/jolt-physics/Build)
target_link_libraries(${PROJECT_NAME} PRIVATE Jolt)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${MAXDOF_PATH_DEPENDENCIES}/jolt-physics
    )
