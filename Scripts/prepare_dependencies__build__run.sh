#!/bin/sh

# Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
# https://github/cortl0/MaxDoF
# Licensed under the GNU Affero General Public License v3.0 or later
# See LICENSE file in the project root for full license information

# Usage:
# Note: This file must be run from the ./Scripts/ directory
# ./prepare_dependencies__build__run.sh

MAXDOF_PATH=$(pwd)/..
MAXDOF_PATH_DEPENDENCIES=${MAXDOF_PATH}/../MaxDoF-Dependencies

MAXDOF_DIR_JOLT_PHYSICS="jolt-physics"
MAXDOF_DIR_OGRE_NEXT_DEPS="ogre-next-deps"
MAXDOF_DIR_OGRE_NEXT="ogre-next"

MAXDOF_PATH_JOLT_PHYSICS="${MAXDOF_PATH_DEPENDENCIES}/${MAXDOF_DIR_JOLT_PHYSICS}"
MAXDOF_PATH_OGRE_NEXT_DEPS="${MAXDOF_PATH_DEPENDENCIES}/${MAXDOF_DIR_OGRE_NEXT_DEPS}"
MAXDOF_PATH_OGRE_NEXT="${MAXDOF_PATH_DEPENDENCIES}/${MAXDOF_DIR_OGRE_NEXT}"

SIMD_CXX_FLAGS="-msse4.1 -msse4.2 -mavx -mavx2 -mfma -mf16c -mlzcnt -mbmi"

echo "MAXDOF_PATH_OGRE_NEXT: ${MAXDOF_PATH_OGRE_NEXT}"
echo "MAXDOF_PATH_OGRE_NEXT_DEPS: ${MAXDOF_PATH_OGRE_NEXT_DEPS}"
echo "MAXDOF_PATH_JOLT_PHYSICS: ${MAXDOF_PATH_JOLT_PHYSICS}"

sudo apt update || exit $?
echo "Installing dependencies of OGRE Next"
sudo apt-get install -y \
    git mingw-w64 wget tar wine64 cmake ninja-build doxygen graphviz python3-clang \
    libfreetype6-dev libfreeimage-dev libzzip-dev libxrandr-dev libxaw7-dev freeglut3-dev libgl1-mesa-dev \
    libglu1-mesa-dev libxcb-randr0-dev libx11-xcb-dev libxcb-keysyms1-dev libsdl2-dev \
    || exit $?

echo "Switching MinGW-w64 to POSIX threads"
sudo update-alternatives --set x86_64-w64-mingw32-gcc /usr/bin/x86_64-w64-mingw32-gcc-posix || exit $?
sudo update-alternatives --set x86_64-w64-mingw32-g++ /usr/bin/x86_64-w64-mingw32-g++-posix || exit $?

mkdir -p ${MAXDOF_PATH_DEPENDENCIES}

# 1. Клонируем исходники зависимостей один раз
if [ ! -d "${MAXDOF_PATH_JOLT_PHYSICS}" ]; then
    echo "Cloning https://github.com/jrouwe/joltphysics"
    git clone https://github.com/jrouwe/joltphysics ${MAXDOF_PATH_JOLT_PHYSICS} || exit $?
    cd ${MAXDOF_PATH_JOLT_PHYSICS}
    git checkout v5.6.0
fi

if [ ! -d "${MAXDOF_PATH_OGRE_NEXT_DEPS}" ]; then
    echo "Cloning https://github.com/OGRECave/ogre-next-deps"
    git clone --recurse-submodules --shallow-submodules https://github.com/OGRECave/ogre-next-deps "${MAXDOF_PATH_OGRE_NEXT_DEPS}" || exit $?
    cd ${MAXDOF_PATH_OGRE_NEXT_DEPS}
    git checkout 20dd36410a81a2b05214cf37f99e58e00810a291
fi

if [ ! -d "${MAXDOF_PATH_OGRE_NEXT}" ]; then
    echo "Cloning https://github.com/OGRECave/ogre-next"
    git clone https://github.com/OGRECave/ogre-next ${MAXDOF_PATH_OGRE_NEXT} || exit $?
    cd ${MAXDOF_PATH_OGRE_NEXT}
    git checkout c13fa29e69b5f4deb9ef88f145fda239273f88c0
fi

MAXDOF_PATH_SDL2_WIN="${MAXDOF_PATH_DEPENDENCIES}/sdl2-windows"
if [ ! -d "${MAXDOF_PATH_SDL2_WIN}" ]; then
    echo "Downloading SDL2 development libraries for MinGW"
    mkdir -p "${MAXDOF_PATH_SDL2_WIN}"
    wget -qO /tmp/SDL2-mingw.tar.gz "https://libsdl.org/release/SDL2-devel-2.30.11-mingw.tar.gz" || exit $?
    tar -xzf /tmp/SDL2-mingw.tar.gz -C "${MAXDOF_PATH_SDL2_WIN}" --strip-components=1 || exit $?
    rm /tmp/SDL2-mingw.tar.gz
fi

DXMATH_FILE="${MAXDOF_PATH_OGRE_NEXT}/OgreMain/src/WIN32/OgreOptimisedUtilDirectXMath.cpp"
if [ -f "${DXMATH_FILE}" ] && ! grep -q "PATCHED_FOR_MINGW" "${DXMATH_FILE}"; then
    sed -i '1i // PATCHED_FOR_MINGW\n#ifdef _MSC_VER' "${DXMATH_FILE}"
    cat << 'EOF' >> "${DXMATH_FILE}"
#else
namespace Ogre { class OptimisedUtil; extern OptimisedUtil* _getOptimisedUtilDirectXMath() { return 0; } }
#endif
EOF
fi

# 2. Собираем зависимости и проект для всех целевых платформ
for TARGET_PLATFORM in "linux-x86_64" "windows-x86_64"; do
    echo "=========================================="
    echo "Building for platform: ${TARGET_PLATFORM}"
    echo "=========================================="

    if [ "${TARGET_PLATFORM}" = "windows-x86_64" ]; then
        CMAKE_TOOLCHAIN_ARG=" \
        -DCMAKE_TOOLCHAIN_FILE=${MAXDOF_PATH}/CMake/Toolchains/mingw-w64-x86_64.cmake
    "
        # PLATFORM_CXX_FLAGS="${SIMD_CXX_FLAGS} -U_WIN32_WINNT -D_WIN32_WINNT=0x0A00 -DWINVER=0x0A00"
        PLATFORM_CXX_FLAGS=" \
            ${SIMD_CXX_FLAGS} \
            -U_WIN32_WINNT \
            -D_WIN32_WINNT=0x0A00 \
            -DWINVER=0x0A00 \
    "
        OGRE_EXTRA_FLAGS=" \
            -DOGRE_BUILD_RENDERSYSTEM_D3D11:BOOL=OFF \
            -DOGRE_CONFIG_ENABLE_STBI:BOOL=ON \
            -DOGRE_BUILD_SAMPLES2:BOOL=OFF \
            -DOGRE_CONFIG_ENABLE_DIRECTXMATH:BOOL=OFF \
            -DOGRE_SIMD_DIRECTXMATH:BOOL=OFF \
            -DOGRE_PROFILING_PROVIDER:STRING=none \
            -DOGRE_PROFILING:BOOL=OFF \
    "
            #  -DOGRE_CONFIG_FREEIMAGE:BOOL=OFF
    else
        CMAKE_TOOLCHAIN_ARG=""
        PLATFORM_CXX_FLAGS="${SIMD_CXX_FLAGS}"
        OGRE_EXTRA_FLAGS="-DOGRE_BUILD_SAMPLES2:BOOL=ON"
    fi

    # Jolt Physics
    JOLT_BUILD_DIR="${MAXDOF_PATH_JOLT_PHYSICS}/build/${TARGET_PLATFORM}/Debug"
    if [ ! -d "${JOLT_BUILD_DIR}" ]; then
        echo "Build Jolt Physics (Debug) for ${TARGET_PLATFORM}"
        mkdir -p "${JOLT_BUILD_DIR}"
        cd "${JOLT_BUILD_DIR}"
        cmake ../../../Build \
            ${CMAKE_TOOLCHAIN_ARG} \
            -DCMAKE_BUILD_TYPE=Debug \
            -DJPH_BUILD_SHARED_LIBS=ON \
            -DDOUBLE_PRECISION=ON \
            -DDEBUG_RENDERER_IN_DISTRIBUTION=OFF \
            -DPROFILER_IN_DISTRIBUTION=OFF \
            -DOVERRIDE_CXX_FLAGS=OFF \
            -DJPH_USE_DX12=OFF \
            -DUSE_SSE4_1=ON \
            -DUSE_SSE4_2=ON \
            -DUSE_AVX=ON \
            -DUSE_AVX2=ON \
            -DUSE_AVX512=OFF \
            -DUSE_LZCNT=ON \
            -DUSE_TZCNT=ON \
            -DUSE_F16C=ON \
            -DUSE_FMADD=ON || exit $?
        make -j12 || exit $?
    fi

    # OGRE-Next Dependencies
    OGRE_DEPS_BUILD_DIR="${MAXDOF_PATH_OGRE_NEXT_DEPS}/build/${TARGET_PLATFORM}"
    if [ ! -d "${OGRE_DEPS_BUILD_DIR}" ]; then
        mkdir -p "${OGRE_DEPS_BUILD_DIR}"
        cd "${OGRE_DEPS_BUILD_DIR}"
        echo "Build dependencies of OGRE Next for ${TARGET_PLATFORM}"
        if [ "${TARGET_PLATFORM}" = "windows-x86_64" ]; then
            sed -i 's/add_subdirectory(FreeImage)/if(NOT WIN32)\n  add_subdirectory(FreeImage)\nendif()/' "${MAXDOF_PATH_OGRE_NEXT_DEPS}/src/CMakeLists.txt"
            cmake ../.. \
                ${CMAKE_TOOLCHAIN_ARG} \
                -DOGREDEPS_BUILD_FREEIMAGE:BOOL=OFF \
                -DOGREDEPS_BUILD_SDL2:BOOL=OFF \
                -DOGREDEPS_BUILD_SHADERC:BOOL=OFF \
                -DOGREDEPS_BUILD_REMOTERY:BOOL=OFF \
                || exit $?
        else
            cmake ../.. ${CMAKE_TOOLCHAIN_ARG} || exit $?
        fi
        make -j12 || exit $?
        echo "Installing dependencies of OGRE Next"
        make install || exit $?
    fi

    # OGRE-Next
    OGRE_BUILD_DIR="${MAXDOF_PATH_OGRE_NEXT}/build/${TARGET_PLATFORM}/Debug"
    if [ ! -d "${OGRE_BUILD_DIR}" ]; then
        echo "Build OGRE Next (Debug) for ${TARGET_PLATFORM}"
        mkdir -p "${OGRE_BUILD_DIR}"
        cd "${OGRE_BUILD_DIR}"
        cmake \
            -D OGRE_STATIC=OFF \
            -D OGRE_CONFIG_THREAD_PROVIDER=0 \
            -D OGRE_CONFIG_THREADS=0 \
            -D OGRE_DEPENDENCIES_DIR="${OGRE_DEPS_BUILD_DIR}/ogredeps" \
            -D CMAKE_BUILD_TYPE=Debug \
            -D CMAKE_CXX_FLAGS="${PLATFORM_CXX_FLAGS}" \
            ${OGRE_EXTRA_FLAGS} \
            ${CMAKE_TOOLCHAIN_ARG} \
            ../../.. || exit $?
        make -j12 || exit $?
    fi

    # MaxDoF
    echo "Build MaxDoF for ${TARGET_PLATFORM}"
    MAXDOF_BUILD_DIR="${MAXDOF_PATH}/build/${TARGET_PLATFORM}/Debug"
    mkdir -p "${MAXDOF_BUILD_DIR}"
    cd "${MAXDOF_BUILD_DIR}"
    cmake -DCMAKE_BUILD_TYPE=Debug ${CMAKE_TOOLCHAIN_ARG} ../../.. || exit $?
    make -j12 || exit $?
done

echo "Run MaxDoF (Linux)"
cd ${MAXDOF_PATH}/bin/linux-x86_64/Debug
./MaxDoFServer
./MaxDoFGame
