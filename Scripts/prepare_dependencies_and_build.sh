#!/bin/sh

# Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
# https://github/cortl0/MaxDoF
# Licensed under the GNU Affero General Public License v3.0 or later
# See LICENSE file in the project root for full license information

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
cd "${SCRIPT_DIR}" || exit $?

if [ "$#" -ne 2 ]; then
    echo "Error: Invalid number of arguments."
    echo "Usage: $0 <linux-x86_64|windows-x86_64> <Debug|Release>"
    exit 1
fi

MAXDOF_TARGET_PLATFORM="$1"
MAXDOF_BUILD_TYPE="$2"

case "${MAXDOF_TARGET_PLATFORM}" in
    linux-x86_64|windows-x86_64)
        ;;
    *)
        echo "Error: Unsupported platform '${MAXDOF_TARGET_PLATFORM}'."
        echo "Allowed values: linux-x86_64, windows-x86_64"
        exit 1
        ;;
esac

case "${MAXDOF_BUILD_TYPE}" in
    Debug|Release)
        ;;
    *)
        echo "Error: Unsupported build type '${MAXDOF_BUILD_TYPE}'."
        echo "Allowed values: Debug, Release"
        exit 1
        ;;
esac

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

MAXDOF_PATH_VULKAN_WIN="${MAXDOF_PATH_DEPENDENCIES}/vulkan-windows"
if [ ! -d "${MAXDOF_PATH_VULKAN_WIN}" ]; then
    echo "Preparing Vulkan headers and import library for MinGW"
    mkdir -p "${MAXDOF_PATH_VULKAN_WIN}/lib"
    git clone --depth 1 --branch v1.3.296 https://github.com/KhronosGroup/vulkan-headers "${MAXDOF_PATH_VULKAN_WIN}/headers" || exit $?
    gendef - /usr/lib/x86_64-linux-gnu/wine/x86_64-windows/vulkan-1.dll > "${MAXDOF_PATH_VULKAN_WIN}/lib/vulkan-1.def" || exit $?
    x86_64-w64-mingw32-dlltool -d "${MAXDOF_PATH_VULKAN_WIN}/lib/vulkan-1.def" -l "${MAXDOF_PATH_VULKAN_WIN}/lib/libvulkan-1.dll.a" || exit $?
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

MAXDOF_PATH_GLSLANG_WIN="${MAXDOF_PATH_DEPENDENCIES}/glslang-windows"
if [ ! -d "${MAXDOF_PATH_GLSLANG_WIN}/src" ]; then
    echo "Cloning glslang for Windows (MinGW)"
    mkdir -p "${MAXDOF_PATH_GLSLANG_WIN}"
    git clone --depth 1 --branch 14.3.0 https://github.com/khronosGroup/glslang "${MAXDOF_PATH_GLSLANG_WIN}/src" || exit $?
fi

# 2. Собираем зависимости и проект для всех целевых платформ
    echo "=========================================="
    echo "Building for platform: ${MAXDOF_TARGET_PLATFORM}"
    echo "=========================================="

    if [ "${MAXDOF_TARGET_PLATFORM}" = "windows-x86_64" ]; then
        CMAKE_TOOLCHAIN_ARG=" \
            -DCMAKE_TOOLCHAIN_FILE=${MAXDOF_PATH}/CMake/Toolchains/mingw-w64-x86_64.cmake \
        "
        PLATFORM_CXX_FLAGS=" \
            ${SIMD_CXX_FLAGS} \
            -U_WIN32_WINNT \
            -D_WIN32_WINNT=0x0A00 \
            -DWINVER=0x0A00 \
        "
        OGREDEPS_WIN_DIR="${MAXDOF_PATH_OGRE_NEXT_DEPS}/build/windows-x86_64/ogredeps"
        GLSLANG_LIBS="${OGREDEPS_WIN_DIR}/lib/libglslang.a;${OGREDEPS_WIN_DIR}/lib/libMachineIndependent.a;${OGREDEPS_WIN_DIR}/lib/libGenericCodeGen.a;${OGREDEPS_WIN_DIR}/lib/libOSDependent.a;${OGREDEPS_WIN_DIR}/lib/libSPIRV.a"

        OGRE_EXTRA_FLAGS=" \
            -DOGRE_BUILD_RENDERSYSTEM_D3D11:BOOL=OFF \
            -DOGRE_CONFIG_ENABLE_STBI:BOOL=ON \
            -DOGRE_BUILD_SAMPLES2:BOOL=OFF \
            -DOGRE_CONFIG_ENABLE_FREEIMAGE:BOOL=OFF \
            -DOGRE_CONFIG_ENABLE_DIRECTXMATH:BOOL=OFF \
            -DOGRE_SIMD_DIRECTXMATH:BOOL=OFF \
            -DOGRE_PROFILING_PROVIDER:STRING=none \
            -DOGRE_PROFILING:BOOL=OFF \
            -DOGRE_BUILD_RENDERSYSTEM_VULKAN:BOOL=ON \
            -DOGRE_BUILD_COMPONENT_OVERLAY:BOOL=ON \
            -DVulkan_INCLUDE_DIR=${MAXDOF_PATH_VULKAN_WIN}/headers/include \
            -DVulkan_LIBRARY=${MAXDOF_PATH_VULKAN_WIN}/lib/libvulkan-1.dll.a \
            -DVulkan_SHADERC_INCLUDE_DIR=${OGREDEPS_WIN_DIR}/include \
            -DVulkan_SHADERC_LIB_REL=${GLSLANG_LIBS} \
            -DVulkan_SHADERC_LIB_DBG=${GLSLANG_LIBS} \
        "
    else
        CMAKE_TOOLCHAIN_ARG=""
        PLATFORM_CXX_FLAGS="${SIMD_CXX_FLAGS}"
        OGRE_EXTRA_FLAGS="-DOGRE_BUILD_SAMPLES2:BOOL=ON"
    fi

    # Jolt Physics
    JOLT_BUILD_DIR="${MAXDOF_PATH_JOLT_PHYSICS}/build/${MAXDOF_TARGET_PLATFORM}/${MAXDOF_BUILD_TYPE}"
    if [ ! -d "${JOLT_BUILD_DIR}" ]; then
        echo "Build Jolt Physics (${MAXDOF_BUILD_TYPE}) for ${MAXDOF_TARGET_PLATFORM}"
        mkdir -p "${JOLT_BUILD_DIR}"
        cd "${JOLT_BUILD_DIR}"
        if [ "${MAXDOF_BUILD_TYPE}" = "Debug" ]; then
            JOLT_DEBUG_FLAGS="-DDEBUG_RENDERER_IN_DEBUG_AND_RELEASE=ON -DPROFILER_IN_DEBUG_AND_RELEASE=ON"
        else
            JOLT_DEBUG_FLAGS="-DDEBUG_RENDERER_IN_DEBUG_AND_RELEASE=OFF -DPROFILER_IN_DEBUG_AND_RELEASE=OFF"
        fi
        cmake ../../../Build \
            ${CMAKE_TOOLCHAIN_ARG} \
            ${JOLT_DEBUG_FLAGS} \
            -DCMAKE_BUILD_TYPE=${MAXDOF_BUILD_TYPE} \
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
    OGRE_DEPS_BUILD_DIR="${MAXDOF_PATH_OGRE_NEXT_DEPS}/build/${MAXDOF_TARGET_PLATFORM}"
    if [ ! -d "${OGRE_DEPS_BUILD_DIR}" ]; then
        mkdir -p "${OGRE_DEPS_BUILD_DIR}"
        cd "${OGRE_DEPS_BUILD_DIR}"
        echo "Build dependencies of OGRE Next for ${MAXDOF_TARGET_PLATFORM}"
        if [ "${MAXDOF_TARGET_PLATFORM}" = "windows-x86_64" ]; then
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
    if [ "${MAXDOF_TARGET_PLATFORM}" = "windows-x86_64" ]; then
        GLSLANG_DIR="${MAXDOF_PATH_DEPENDENCIES}/glslang-windows"
        OGREDEPS_DIR="${MAXDOF_PATH_OGRE_NEXT_DEPS}/build/windows-x86_64/ogredeps"
        if [ ! -f "${OGREDEPS_DIR}/lib/libglslang.a" ]; then
            mkdir -p "${GLSLANG_DIR}/build"
            cd "${GLSLANG_DIR}/build"
            cmake ../src \
                ${CMAKE_TOOLCHAIN_ARG} \
                -DCMAKE_BUILD_TYPE=Release \
                -DCMAKE_INSTALL_PREFIX="${OGREDEPS_DIR}" \
                -DENABLE_OPT=OFF \
                -DENABLE_GLSLANG_BINARIES=OFF \
                -DENABLE_CTEST=OFF \
                -DALLOW_EXTERNAL_SPIRV_TOOLS=OFF || exit $?
            make -j12 || exit $?
            make install || exit $?
        fi
        mkdir -p "${OGREDEPS_DIR}/lib/Debug" "${OGREDEPS_DIR}/lib/Release"
        cp -n "${OGREDEPS_DIR}/lib/Release/"*.a "${OGREDEPS_DIR}/lib/Debug/" 2>/dev/null || true
        cp -n "${OGREDEPS_DIR}/lib/Release/"*.a "${OGREDEPS_DIR}/lib/" 2>/dev/null || true
        cp -n "${OGREDEPS_DIR}/lib/"*.a "${OGREDEPS_DIR}/lib/Debug/" 2>/dev/null || true
        cp -n "${OGREDEPS_DIR}/lib/"*.a "${OGREDEPS_DIR}/lib/Release/" 2>/dev/null || true
    fi

    # OGRE-Next
    OGRE_BUILD_DIR="${MAXDOF_PATH_OGRE_NEXT}/build/${MAXDOF_TARGET_PLATFORM}/${MAXDOF_BUILD_TYPE}"
    if [ "${BUILD_TYPE}" = "Debug" ]; then
        OGRE_SUFFIX="_d"
    else
        OGRE_SUFFIX=""
    fi
    if [ "${TARGET_PLATFORM}" = "windows-x86_64" ]; then
        OGRE_READY_CHECK="${OGRE_BUILD_DIR}/bin/RenderSystem_Vulkan${OGRE_SUFFIX}.dll"
    else
        OGRE_READY_CHECK="${OGRE_BUILD_DIR}/lib/RenderSystem_Vulkan${OGRE_SUFFIX}.so"
    fi
    if [ ! -f "${OGRE_READY_CHECK}" ]; then
        echo "Build OGRE Next (${MAXDOF_BUILD_TYPE}) for ${MAXDOF_TARGET_PLATFORM}"
        mkdir -p "${OGRE_BUILD_DIR}"
        cd "${OGRE_BUILD_DIR}"
        rm -f CMakeCache.txt
        cmake \
            -D OGRE_STATIC=OFF \
            -D OGRE_CONFIG_THREAD_PROVIDER=0 \
            -D OGRE_CONFIG_THREADS=0 \
            -D OGRE_DEPENDENCIES_DIR="${OGRE_DEPS_BUILD_DIR}/ogredeps" \
            -D CMAKE_BUILD_TYPE=${MAXDOF_BUILD_TYPE} \
            -D CMAKE_CXX_FLAGS="${PLATFORM_CXX_FLAGS}" \
            ${OGRE_EXTRA_FLAGS} \
            ${CMAKE_TOOLCHAIN_ARG} \
            ../../.. || exit $?
        make -j12 || exit $?
    fi

    # MaxDoF
    echo "Build MaxDoF for ${MAXDOF_TARGET_PLATFORM}"
    MAXDOF_BUILD_DIR="${MAXDOF_PATH}/build/${MAXDOF_TARGET_PLATFORM}/${MAXDOF_BUILD_TYPE}"
    mkdir -p "${MAXDOF_BUILD_DIR}"
    cd "${MAXDOF_BUILD_DIR}"
    cmake -DCMAKE_BUILD_TYPE=${MAXDOF_BUILD_TYPE} -DMAXDOF_TARGET_PLATFORM=${MAXDOF_TARGET_PLATFORM} ${CMAKE_TOOLCHAIN_ARG} ../../.. || exit $?
    make -j12 || exit $?
