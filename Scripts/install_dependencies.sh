#!/bin/sh

# Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
# https://github/cortl0/MaxDoF
# Licensed under the GNU Affero General Public License v3.0 or later
# See LICENSE file in the project root for full license information

sudo apt update || exit $?
sudo apt-get install -y \
    git tar wget cmake ninja-build doxygen python3-clang graphviz mingw-w64 mingw-w64-tools wine64 \
    libfreetype6-dev libfreeimage-dev libzzip-dev libxrandr-dev libxaw7-dev freeglut3-dev libgl1-mesa-dev \
    libglu1-mesa-dev libxcb-randr0-dev libx11-xcb-dev libxcb-keysyms1-dev libsdl2-dev \
    || exit $?
    
echo "Switching MinGW-w64 to POSIX threads"
sudo update-alternatives --set x86_64-w64-mingw32-gcc /usr/bin/x86_64-w64-mingw32-gcc-posix || exit $?
sudo update-alternatives --set x86_64-w64-mingw32-g++ /usr/bin/x86_64-w64-mingw32-g++-posix || exit $?
