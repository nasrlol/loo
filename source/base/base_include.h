#ifndef BASE_INCLUDE_H
#define BASE_INCLUDE_H

#if defined(__linux__) ||  defined(__APPLE__)
// #message "PLATFORM LINUX APPLE"
#define PLATFORM_UNIX 1
#define PLATFORM_LINUX 1
#elif defined(__linux__) && !defined(__APPLE__)
#message "PLATFORM LINUX"
#define PLATFORM_LINUX 1
#elif defined(_WIN32) || defined(_WIN64)
#message "PLATFORM WINDOWS"
#define PLATFORM_WINDOWS 1
#elif defined(__APPLE__)
#define PLATFORM_APPLE 1
#message "PLATFORM APPLE"
#else
#message platform unknown
#endif // platform definitions

#if PLATFORM_UNIX
#include <unistd.h>
#include <dirent.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <sys/socket.h> // needed for making a connection to displays (wayland)
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <fcntl.h>
#include <dlfcn.h>

#if DEBUG
#include <errno.h>
#endif

#if PLATFORM_LINUX

#if X11
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#elif WAYLAND
#include <wayland-client.h>
#endif

#endif // PLATFORM_LINUX
#elif PLATFORM_WINDOWS
#define _WIN32_WINNT 0x0601
#include <windows.h>
#endif // PLATFORM_UNIX - PLATFORM_WINDOWS

#if VULKAN
// #include <vulkan/vulkan.h>
#elif OPENGL
...
#endif

//-

#include <time.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>

//-
#include "base_core.h"
#include "base_memory.h"

#include "base_arena.h"
#include "base_stack.h"
#include "base_string.h"
#include "base_platform.h"
#include "base_render.h"

#include "base_memory.cpp"
#include "base_arena.cpp"
#include "base_stack.cpp"
#include "base_string.cpp"
#include "base_platform.cpp"

#include "base_rand.cpp"
#include "base_simd.cpp"

#if VULKAN

#include "base_render_vulkan.cpp"
#endif

#endif // BASE_INCLUDE_H
