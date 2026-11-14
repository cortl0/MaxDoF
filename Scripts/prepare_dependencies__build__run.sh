#!/bin/sh

# Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
# https://github/cortl0/MaxDoF
# Licensed under the GNU Affero General Public License v3.0 or later
# See LICENSE file in the project root for full license information

# Usage:
# Note: This file must be run from the ./Scripts/ directory
# ./prepare_dependencies__build__run.sh [path-jolt-physics [path-ogre-next-deps [path-ogre-next]]]

MAXDOF_PATH=$(pwd)/..
MAXDOF_PATH_DEPENDENCIES=${MAXDOF_PATH}/../MaxDoF-Dependencies

MAXDOF_DIR_JOLT_PHYSICS="jolt-physics"
MAXDOF_DIR_OGRE_NEXT_DEPS="ogre-next-deps"
MAXDOF_DIR_OGRE_NEXT="ogre-next"

MAXDOF_DEFAULT_PATH_JOLT_PHYSICS="${MAXDOF_PATH_DEPENDENCIES}/${MAXDOF_DIR_JOLT_PHYSICS}"
MAXDOF_DEFAULT_PATH_OGRE_NEXT_DEPS="${MAXDOF_PATH_DEPENDENCIES}/${MAXDOF_DIR_OGRE_NEXT_DEPS}"
MAXDOF_DEFAULT_PATH_OGRE_NEXT="${MAXDOF_PATH_DEPENDENCIES}/${MAXDOF_DIR_OGRE_NEXT}"

MAXDOF_PATH_JOLT_PHYSICS="${1:-${MAXDOF_DEFAULT_PATH_JOLT_PHYSICS}}"
MAXDOF_PATH_OGRE_NEXT_DEPS="${2:-${MAXDOF_DEFAULT_PATH_OGRE_NEXT_DEPS}}"
MAXDOF_PATH_OGRE_NEXT="${3:-${MAXDOF_DEFAULT_PATH_OGRE_NEXT}}"

SIMD_CXX_FLAGS="-msse4.1 -msse4.2 -mavx -mavx2 -mfma -mf16c -mlzcnt -mbmi"

echo "MAXDOF_PATH_OGRE_NEXT: ${MAXDOF_PATH_OGRE_NEXT}"
echo "MAXDOF_PATH_OGRE_NEXT_DEPS: ${MAXDOF_PATH_OGRE_NEXT_DEPS}"
echo "MAXDOF_PATH_JOLT_PHYSICS: ${MAXDOF_PATH_JOLT_PHYSICS}"

sudo apt update || exit $?
sudo apt-get install -y git || exit $?
echo "Installing dependencies of OGRE Next"
sudo apt-get install -y libfreetype6-dev libfreeimage-dev libzzip-dev libxrandr-dev libxaw7-dev freeglut3-dev libgl1-mesa-dev libglu1-mesa-dev libxcb-randr0-dev libx11-xcb-dev libxcb-keysyms1-dev doxygen graphviz python3-clang libsdl2-dev cmake ninja-build || exit $?

mkdir -p ${MAXDOF_PATH_DEPENDENCIES}

if [ ! -d "${MAXDOF_PATH_JOLT_PHYSICS}" ]; then
    echo "Cloning https://github.com/jrouwe/joltphysics"
    git clone https://github.com/jrouwe/joltphysics ${MAXDOF_PATH_JOLT_PHYSICS} || exit $?
    cd ${MAXDOF_PATH_JOLT_PHYSICS}
    git checkout v5.6.0
    mkdir -p build

    echo "Build Jolt Physics (Debug)"
    cd ${MAXDOF_PATH_JOLT_PHYSICS}/build
    mkdir -p Debug
    cd Debug
    cmake ../../Build \
        -DCMAKE_BUILD_TYPE=Debug \
        -DDOUBLE_PRECISION=ON \
        -DDEBUG_RENDERER_IN_DISTRIBUTION=OFF \
        -DPROFILER_IN_DISTRIBUTION=OFF \
        -DOVERRIDE_CXX_FLAGS=OFF \
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

    # Build Release
    #cd ${MAXDOF_PATH_JOLT_PHYSICS}/build
    #mkdir -p Release
    #cd Release
    #cmake ../../Build \
    #    -DCMAKE_BUILD_TYPE=Release \
    #    -DDOUBLE_PRECISION=ON \
    #    -DDEBUG_RENDERER_IN_DISTRIBUTION=OFF \
    #    -DPROFILER_IN_DISTRIBUTION=OFF \
    #    -DOVERRIDE_CXX_FLAGS=OFF \
    #    -DUSE_SSE4_1=ON \
    #    -DUSE_SSE4_2=ON \
    #    -DUSE_AVX=ON \
    #    -DUSE_AVX2=ON \
    #    -DUSE_AVX512=OFF \
    #    -DUSE_LZCNT=ON \
    #    -DUSE_TZCNT=ON \
    #    -DUSE_F16C=ON \
    #    -DUSE_FMADD=ON || exit $?
    #make -j12 || exit $?
fi

if [ ! -d "${MAXDOF_PATH_OGRE_NEXT_DEPS}" ]; then
    echo "Cloning https://github.com/OGRECave/ogre-next-deps"
    git clone --recurse-submodules --shallow-submodules https://github.com/OGRECave/ogre-next-deps "${MAXDOF_PATH_OGRE_NEXT_DEPS}" || exit $?
    cd ${MAXDOF_PATH_OGRE_NEXT_DEPS}
    git checkout f7e23191f4fd89ed41134996bc47f9ee23d009df
    mkdir -p build
    cd build
    echo "Build dependencies of OGRE Next"
    cmake .. || exit $?
    make -j12 || exit $?
    echo "Installing dependencies of OGRE Next"
    make install || exit $?
fi

if [ ! -d "${MAXDOF_PATH_OGRE_NEXT}" ]; then
    echo "Cloning https://github.com/OGRECave/ogre-next"
    git clone https://github.com/OGRECave/ogre-next ${MAXDOF_PATH_OGRE_NEXT} || exit $?
    cd ${MAXDOF_PATH_OGRE_NEXT}
    git checkout 475f783d76f21b530e718aab4a548cdd9a222e50
    ln -s ${MAXDOF_PATH_OGRE_NEXT_DEPS}/build/ogredeps Dependencies
    mkdir -p build
    echo "Build OGRE Next"

    # Build Debug
    cd ${MAXDOF_PATH_OGRE_NEXT}/build
    mkdir -p Debug
    cd Debug
    cmake -D OGRE_BUILD_SAMPLES2=1 \
          -D OGRE_CONFIG_THREAD_PROVIDER=0 \
          -D OGRE_CONFIG_THREADS=0 \
          -D CMAKE_BUILD_TYPE=Debug \
          -D CMAKE_CXX_FLAGS="${SIMD_CXX_FLAGS}" \
          ../.. || exit $?
    make -j12 || exit $?
    #make install || exit $?

    # Build Release
    #cd ${MAXDOF_PATH_OGRE_NEXT}/build
    #mkdir -p Release
    #cd Release
    #cmake -D OGRE_BUILD_SAMPLES2=1 \
    #      -D OGRE_CONFIG_THREAD_PROVIDER=0 \
    #      -D OGRE_CONFIG_THREADS=0 \
    #      -D CMAKE_BUILD_TYPE=Release \
    #      -D CMAKE_CXX_FLAGS="${SIMD_CXX_FLAGS}" \
    #      ../.. || exit $?
    #make -j12 || exit $?
    #make install || exit $?
fi

if [ ! -d "${MAXDOF_DEFAULT_PATH_JOLT_PHYSICS}" ]; then
    ln -s ${MAXDOF_PATH_JOLT_PHYSICS} ${MAXDOF_DEFAULT_PATH_JOLT_PHYSICS}
fi
if [ ! -d "${MAXDOF_DEFAULT_PATH_OGRE_NEXT_DEPS}" ]; then
    ln -s ${MAXDOF_PATH_OGRE_NEXT_DEPS} ${MAXDOF_DEFAULT_PATH_OGRE_NEXT_DEPS}
fi
if [ ! -d "${MAXDOF_DEFAULT_PATH_OGRE_NEXT}" ]; then
    ln -s ${MAXDOF_PATH_OGRE_NEXT} ${MAXDOF_DEFAULT_PATH_OGRE_NEXT}
fi

echo "Build MaxDoF"
mkdir -p ${MAXDOF_PATH}/build
cd ${MAXDOF_PATH}/build
cmake -DCMAKE_BUILD_TYPE=Debug .. || exit $?
make -j12 || exit $?

echo "Run MaxDoF"
cd ${MAXDOF_PATH}/bin/Debug
./MaxDoFServer
./MaxDoFGame
