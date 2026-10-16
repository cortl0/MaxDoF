# https://Github/cortl0/MaxDoF
# GPL-3.0 license

cmake_minimum_required( VERSION 3.5.1 )
project(MaxDoFServer)

file(GLOB_RECURSE SRC_FILES_TEMP CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
set(SRC_FILES ${SRC_FILES} ${SRC_FILES_TEMP})
file(GLOB_RECURSE SRC_FILES_TEMP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Scenes/*.cpp")
set(SRC_FILES ${SRC_FILES}
    ${SRC_FILES_TEMP}
    ${CMAKE_SOURCE_DIR}/src/World/Manager.cpp
    ${CMAKE_SOURCE_DIR}/src/World/Physics.cpp
)

set(MAXDOF_PATH_DEPENDENCIES "${CMAKE_SOURCE_DIR}/../MaxDoF-Dependencies")

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_directories(${PROJECT_NAME} PRIVATE ${MAXDOF_PATH_DEPENDENCIES}/jolt-physics/Build)
target_link_libraries(${PROJECT_NAME} PRIVATE Jolt)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${MAXDOF_PATH_DEPENDENCIES}/jolt-physics
    )
