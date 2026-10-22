#ifndef BASE_CORE_H
#define BASE_CORE_H

//- compiler definitions
#if defined(_MSC_VER)
#define COMPILER_MSVC 1
#elif defined(__clang__)
#define COMPILER_CLANG 1
#elif defined(__GNUC__)
#define COMPILER_GCC 1
#else
#error Unsupported compiler
#endif

//- architecture definitions

#if defined(__arm__) || defined(__aarch64__) || defined(_M_ARM64)
#define ARCH_ARM 1
#elif defined(__x86_64__) || defined(_M_X64)
#define ARCH_x86_64 1
#elif defined(__i386__) || defined(_M_IX86)
#define ARCH_X86 1
#endif

#ifndef COMPILER_MSVC
#define COMPILER_MSVC 0
#endif
#ifndef COMPILER_CLANG
#define COMPILER_CLANG 0
#endif
#ifndef COMPILER_GCC
#define COMPILER_GCC 0
#endif
#ifndef ARCH_ARM
#define ARCH_ARM 0
#endif
#ifndef ARCH_x86_64
#define ARCH_x86_64 0
#endif
#ifndef ARCH_X86
#define ARCH_X86 0
#endif

//- compiler definitions

//- core stuff

/* assert an expression and output the file and the line */
#define internal static
#define global_variable static
#define local_persist static

#define arena_align (2 * sizeof(void *))

#define roundup_4(n) (((n) + 3) & -4)

#define Min(a, b) (((a) < (b)) ? (a) : (b))
#define Max(a, b) (((a) > (b)) ? (a) : (b))
#define Clamp(value, low, high) Min(Max((value), (low)), (high))
#define ArrayCount(array) (sizeof(array) / sizeof((array)[0]))

#define Unused(x) (void)(x)
#define Nil 0

#define Deprecated [[deprecated]]

//- breakpoints
#if COMPILER_MSVC
#include <intrin.h>
#define breakpoint() __debugbreak()
#define Trap() __fastfail(0)
#elif COMPILER_CLANG
#define breakpoint() __builtin_debugtrap()
#define Trap() __builtin_trap()
#elif ARCH_ARM
#define breakpoint() __asm__ volatile("brk #0")
#define Trap() __builtin_trap()
#else
#define breakpoint() __asm__ volatile("int3")
#define Trap() __builtin_trap()
#endif

//-
#define MemCpy(dest, src, len) memcpy((dest), (src), (len))
#define MemSet(dest, val, len) memset((dest), (val), (len))
#define MemSetZero(dest, len) memset((dest), (0), (len))

//-
#if COMPILER_MSVC
#pragma section("read_only", read)
#define ReadOnly __declspec(allocate("read_only"))
#elif COMPILER_GCC || COMPILER_CLANG

#if PLATFORM_APPLE
#define ReadOnly __attribute__((section("__TEXT,__const")))
#else
#define ReadOnly __attribute__((section(".const")))
#endif // PLATFORM_APPLE

#endif

//-

//- types

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef int8_t  s8;
typedef int16_t s16;

typedef int32_t s32;
typedef int64_t s64;

typedef float  f32;
typedef double f64;

typedef s32 b32;
typedef s16 b16;
typedef s8  b8;

typedef uintptr_t umm;
typedef intptr_t  smm;

//- disable certain msvc errors
#if COMPILER_MSVC
#pragma warning(disable : 6246) // disable the same name within a certain scope
#endif

//-

#define Assert(expression)   \
do {                     \
  if (!(expression)) { \
  breakpoint();    \
  Trap();          \
  }                    \
  } while (0)

  // NOTE: apparantly in macros this strinifys whataever you put after the #
  #define AssertLog(expression, ...)                                                \
  do {                                                                          \
  if (!(expression)) {                                                      \
  Log("%s | %s:%d: %s", __FUNCTION__, __FILE__, __LINE__, #expression); \
  Log(__VA_ARGS__);                                                     \
  Assert(false);                                                        \
  }                                                                         \
 } while (0)

#endif //  BASE_CORE_H
