# Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
# https://github/cortl0/MaxDoF
# Licensed under the GNU Affero General Public License v3.0 or later
# See LICENSE file in the project root for full license information

function(setup_audio_system TARGET_NAME MAXDOF_PATH_DEPENDENCIES)
    find_package(SDL2 REQUIRED)
    find_program(FAUST_EXECUTABLE faust REQUIRED NO_CMAKE_FIND_ROOT_PATH)

    set(STEAMAUDIO_DIR "${MAXDOF_PATH_DEPENDENCIES}/steam-audio")
    set(FAUST_GENERATED_DIR "${CMAKE_BINARY_DIR}/generated/dsp")
    file(MAKE_DIRECTORY "${FAUST_GENERATED_DIR}")

    # 1. Находим базовую папку, внутри которой лежит подпапка faust/
    find_path(FAUST_INCLUDE_DIR
        NAMES faust/dsp/dsp.h
        PATHS
            "/usr/include"
            "/usr/local/include"
            "${CMAKE_SOURCE_DIR}/third_party/faust/include"
        NO_DEFAULT_PATH
        NO_CMAKE_FIND_ROOT_PATH
        REQUIRED
    )

    # 2. Копируем ТОЛЬКО подпапку faust в изолированную директорию сборки
    set(FAUST_ISOLATED_INCLUDE "${CMAKE_BINARY_DIR}/generated/include")
    file(COPY "${FAUST_INCLUDE_DIR}/faust" DESTINATION "${FAUST_ISOLATED_INCLUDE}")

    # 3. Передаем изолированный путь в target_include_directories вместо /usr/include:
    target_include_directories(${TARGET_NAME} SYSTEM PRIVATE
        "${CMAKE_SOURCE_DIR}/Game/Audio"
        "${FAUST_GENERATED_DIR}"
        ${SDL2_INCLUDE_DIRS}
        "${STEAMAUDIO_DIR}/include"
        "${FAUST_ISOLATED_INCLUDE}"
    )

    if(WIN32 OR MINGW)
        # При кросс-компиляции через MinGW ищем phonon.dll или phonon.lib в папке windows-x64
        find_file(PHONON_LIB
            NAMES phonon.dll phonon.lib libphonon.dll.a
            PATHS
                "${STEAMAUDIO_DIR}/lib/windows-x64"
                "${STEAMAUDIO_DIR}/bin/windows-x64"
            NO_DEFAULT_PATH
            NO_CMAKE_FIND_ROOT_PATH
            REQUIRED
        )
    else()
        # Нативная сборка под Linux
        find_library(PHONON_LIB
            NAMES phonon libphonon
            PATHS
                "${STEAMAUDIO_DIR}/lib/linux-x64"
            NO_DEFAULT_PATH
            REQUIRED
        )
    endif()

    target_link_libraries(${TARGET_NAME} PRIVATE
        ${SDL2_LIBRARIES}
        ${PHONON_LIB}
    )

        # Копируем phonon.dll рядом с .exe при сборке под Windows / MinGW
    if(WIN32 OR MINGW)
        find_file(PHONON_DLL
            NAMES phonon.dll
            PATHS
                "${STEAMAUDIO_DIR}/lib/windows-x64"
                "${STEAMAUDIO_DIR}/bin/windows-x64"
            NO_DEFAULT_PATH
            NO_CMAKE_FIND_ROOT_PATH
            REQUIRED
        )

        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${PHONON_DLL}"
                "$<TARGET_FILE_DIR:${TARGET_NAME}>"
            COMMENT "Copying phonon.dll to output directory"
        )
    endif()

endfunction()

# Генерация .h из .dsp (попадает в build/generated/dsp, поэтому GLOB_RECURSE в src/ их не заденет)
function(add_faust_dsp TARGET_NAME DSP_FILE CLASS_NAME)
    get_filename_component(DSP_ABS_PATH "${DSP_FILE}" ABSOLUTE BASE_DIR "${CMAKE_SOURCE_DIR}")
    set(OUTPUT_HEADER "${CMAKE_BINARY_DIR}/generated/dsp/${CLASS_NAME}.h")

    add_custom_command(
        OUTPUT "${OUTPUT_HEADER}"
        COMMAND ${FAUST_EXECUTABLE} -cn ${CLASS_NAME} -scn dsp "${DSP_ABS_PATH}" -o "${OUTPUT_HEADER}"
        DEPENDS "${DSP_ABS_PATH}"
        COMMENT "Compiling Faust DSP: ${DSP_FILE} -> ${CLASS_NAME}.h"
        VERBATIM
    )
    # Добавляем сгенерированный .h в зависимости цели, чтобы CMake собрал его ДО компиляции .cpp
    target_sources(${TARGET_NAME} PRIVATE "${OUTPUT_HEADER}")
endfunction()
