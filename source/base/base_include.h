#ifndef BASE_INCLUDE_H
#define BASE_INCLUDE_H

#if defined(__APPLE__)
#define PLATFORM_APPLE 1
#elif defined(__linux__)
#define PLATFORM_LINUX 1
#elif defined(_WIN32)
#define PLATFORM_WINDOWS 1
#else
#error Unsupported platform
#endif // platform definitions

#ifndef PLATFORM_APPLE
#define PLATFORM_APPLE 0
#endif
#ifndef PLATFORM_LINUX
#define PLATFORM_LINUX 0
#endif
#ifndef PLATFORM_WINDOWS
#define PLATFORM_WINDOWS 0
#endif
#ifndef DEBUG
#define DEBUG 0
#endif

#if PLATFORM_LINUX
#include <unistd.h>
#include <dirent.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <fcntl.h>
#include <dlfcn.h>

#include <pthread.h>

#elif PLATFORM_WINDOWS
#define _WIN32_WINNT 0x0601
#include <windows.h>
#endif // PLATFORM_LINUX - PLATFORM_WINDOWS

//-
#include <time.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <math.h>

//-
#include "base_core.h"
#include "base_memory.h"

#include "base_arena.h"
#include "base_stack.h"
#include "base_string.h"
#include "base_thread.h"
#include "base_math.h"
#include "base_profile.h"

#if PLATFORM_LINUX
#include "base_arena_linux.h"
#include "base_stack_linux.h"
#include "base_thread_linux.h"
#include "base_platform_linux.h"
#elif PLATFORM_WINDOWS
#include "base_arena_win32.h"
#include "base_stack_win32.h"
#include "base_platform_win32.h"
#else
#error unsupported platform
#endif

#if ARCH_x86_64
#include "base_profile_x64.h"
#elif ARCH_ARM
#include "base_profile_arm.h"
#endif

#include "base_memory.cpp"
#include "base_arena.cpp"
#include "base_stack.cpp"
#include "base_string.cpp"
#include "base_math.cpp"
#include "base_simd.cpp"

#if PLATFORM_LINUX
#include "base_arena_linux.cpp"
#include "base_stack_linux.cpp"
#include "base_thread_linux.cpp"
#include "base_platform_linux.cpp"
#elif PLATFORM_WINDOWS
#include "base_arena_win32.cpp"
#include "base_stack_win32.cpp"
#include "base_platform_win32.cpp"
#endif

#if ARCH_x86_64
#include "base_profile_x64.cpp"
#elif ARCH_ARM
#include "base_profile_arm.cpp"
#else
#error platform not supported for profiling
#endif

#include "base_rand.cpp"

#endif // BASE_INCLUDE_H
