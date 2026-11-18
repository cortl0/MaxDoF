# Целевая система и архитектура
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Указываем кросс-компиляторы MinGW-w64
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

# Правила поиска: программы берем с хоста (Linux), а библиотеки и заголовки — строго из целевого окружения (Windows)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_C_FLAGS_INIT "-U_WIN32_WINNT -D_WIN32_WINNT=0x0A00 -DWINVER=0x0A00")
set(CMAKE_CXX_FLAGS_INIT "-U_WIN32_WINNT -D_WIN32_WINNT=0x0A00 -DWINVER=0x0A00")

set(CMAKE_FIND_ROOT_PATH
    /usr/x86_64-w64-mingw32
    "${CMAKE_CURRENT_LIST_DIR}/../../../MaxDoF-Dependencies/sdl2-windows/x86_64-w64-mingw32"
    "${CMAKE_CURRENT_LIST_DIR}/../../../MaxDoF-Dependencies/ogre-next-deps/build/windows-x86_64/ogredeps"
    "${CMAKE_CURRENT_LIST_DIR}/../../../MaxDoF-Dependencies/vulkan-windows"
)
