#!/bin/sh

# Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
# https://github/cortl0/MaxDoF
# Licensed under the GNU Affero General Public License v3.0 or later
# See LICENSE file in the project root for full license information

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
cd "${SCRIPT_DIR}" || exit $?

MAXDOF_PATH=$(cd "${SCRIPT_DIR}/.." && pwd)
MAXDOF_PATH_DEPENDENCIES=$(cd "${MAXDOF_PATH}/.." && pwd)/MaxDoF-Dependencies

# Читаем версию проекта из корневого CMakeLists.txt
PROJECT_VERSION="v$(grep -E "project\(MaxDoF VERSION" "${MAXDOF_PATH}/CMakeLists.txt" | sed -E 's/.*VERSION ([0-9.]+).*/\1/')"
if [ -z "${PROJECT_VERSION}" ]; then
    echo "PROJECT_VERSION [${PROJECT_VERSION}] is wrong"
    exit 1
fi

DIST_ROOT="${MAXDOF_PATH}/ReleasePackages"
rm -rf "${DIST_ROOT}"
mkdir -p "${DIST_ROOT}"

# Убедимся, что zip установлен для упаковки Windows-архива
if ! command -v zip >/dev/null 2>&1; then
    echo "Installing zip utility..."
    sudo apt-get update && sudo apt-get install -y zip || exit $?
fi

for PLATFORM in "linux-x86_64" "windows-x86_64"; do
    echo "=========================================="
    echo "Packaging Release for ${PLATFORM}"
    echo "=========================================="

    PKG_NAME="MaxDoF-${PROJECT_VERSION}-${PLATFORM}"
    PKG_DIR="${DIST_ROOT}/${PKG_NAME}"
    BIN_SRC="${MAXDOF_PATH}/bin/${PLATFORM}/Release"
    OGRE_BIN="${MAXDOF_PATH_DEPENDENCIES}/ogre-next/build/${PLATFORM}/Release"
    JOLT_BIN="${MAXDOF_PATH_DEPENDENCIES}/jolt-physics/build/${PLATFORM}/Release"

    if [ ! -d "${BIN_SRC}" ]; then
        echo "Error: Release binaries not found at ${BIN_SRC}. Run build_all.sh first."
        exit 1
    fi

    mkdir -p "${PKG_DIR}/Data"

    # 1. Копируем общие ресурсы игры и недостающие медиа из OGRE Samples
    cp -r "${MAXDOF_PATH}/bin/Data/"* "${PKG_DIR}/Data/" || exit $?
    mkdir -p "${PKG_DIR}/Data/Media"
    cp -r "${MAXDOF_PATH_DEPENDENCIES}/ogre-next/Samples/Media/models" "${PKG_DIR}/Data/Media/" || exit $?
    mkdir -p "${PKG_DIR}/Data/Textures"

    # 2. Генерируем автономный resources2.cfg (без ссылок за пределы папки игры)
    cat << 'EOF' > "${PKG_DIR}/resources2.cfg"
[Esseal]
Zip=Data/DebugPack.zip

[General]
FileSystem=Data
FileSystem=Data/2.0/scripts/Materials/Common
FileSystem=Data/2.0/scripts/Materials/Common/Any
FileSystem=Data/2.0/scripts/Materials/Common/GLSL
FileSystem=Data/2.0/scripts/Materials/Common/HLSL
FileSystem=Data/2.0/scripts/Materials/Common/Metal
FileSystem=Data/Media/models
FileSystem=Data/Textures

[Hlms]
DoNotUseAsResource=Data
EOF

    # 3. Копируем бинарники, библиотеки и генерируем plugins.cfg
    if [ "${PLATFORM}" = "windows-x86_64" ]; then
        cp "${BIN_SRC}/MaxDoFGame.exe" "${PKG_DIR}/" || exit $?
        cp "${BIN_SRC}/MaxDoFServer.exe" "${PKG_DIR}/" || exit $?
        cp "${BIN_SRC}/"*.dll "${PKG_DIR}/" || exit $?

        # Срезаем отладочные символы для уменьшения веса релизных файлов
        x86_64-w64-mingw32-strip "${PKG_DIR}/MaxDoFGame.exe" "${PKG_DIR}/MaxDoFServer.exe" "${PKG_DIR}/"*.dll 2>/dev/null || true

        cat << 'EOF' > "${PKG_DIR}/plugins.cfg"
# Defines plugins to load
PluginFolder=.

# Define plugins
Plugin=RenderSystem_GL3Plus
Plugin=RenderSystem_Vulkan
EOF

        # Создаем удобные bat-файлы запуска для Windows
        printf '@echo off\r\ncd /d "%%~dp0"\r\nstart "" "MaxDoFGame.exe" %%*\r\n' > "${PKG_DIR}/run_game.bat"
        printf '@echo off\r\ncd /d "%%~dp0"\r\n"MaxDoFServer.exe" %%*\r\npause\r\n' > "${PKG_DIR}/run_server.bat"

        # Упаковываем в ZIP для Windows
        cd "${DIST_ROOT}"
        zip -r -q "${PKG_NAME}.zip" "${PKG_NAME}" || exit $?
        echo "Created: ${DIST_ROOT}/${PKG_NAME}.zip"
    else
        cp "${BIN_SRC}/MaxDoFGame" "${PKG_DIR}/" || exit $?
        cp "${BIN_SRC}/MaxDoFServer" "${PKG_DIR}/" || exit $?

        # Копируем .so библиотеки Jolt и OGRE-Next (включая симлинки с версиями)
        cp -a "${JOLT_BIN}/libJolt.so"* "${PKG_DIR}/" || exit $?
        cp -a "${OGRE_BIN}/lib/"*.so* "${PKG_DIR}/" || exit $?

        # Срезаем отладочные символы
        strip "${PKG_DIR}/MaxDoFGame" "${PKG_DIR}/MaxDoFServer" "${PKG_DIR}/"*.so* 2>/dev/null || true

        cat << 'EOF' > "${PKG_DIR}/plugins.cfg"
# Defines plugins to load
PluginFolder=.

# Define plugins
Plugin=RenderSystem_GL3Plus
Plugin=RenderSystem_Vulkan
EOF

        # Создаем удобные скрипты запуска для Linux (чтобы .so брались из текущей папки)
        cat << 'EOF' > "${PKG_DIR}/run_game.sh"
#!/bin/sh
cd "$(dirname "$0")" || exit 1
LD_LIBRARY_PATH=".:${LD_LIBRARY_PATH}" ./MaxDoFGame "$@"
EOF
        cat << 'EOF' > "${PKG_DIR}/run_server.sh"
#!/bin/sh
cd "$(dirname "$0")" || exit 1
LD_LIBRARY_PATH=".:${LD_LIBRARY_PATH}" ./MaxDoFServer "$@"
EOF
        chmod +x "${PKG_DIR}/run_game.sh" "${PKG_DIR}/run_server.sh"

        # Упаковываем в tar.gz для Linux
        cd "${DIST_ROOT}"
        tar -czf "${PKG_NAME}.tar.gz" "${PKG_NAME}" || exit $?
        echo "Created: ${DIST_ROOT}/${PKG_NAME}.tar.gz"
    fi
done

echo "All release packages are ready in: ${DIST_ROOT}"
