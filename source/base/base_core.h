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

#if defined(__arm__) || defined(__aarch64__)
#define ARCH_ARM 1
#elif defined(__i386__) || defined(__x86_64__)
#define ARCH_x86_64 1
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

#define Unused(x) (void)(x)
#define Nil 0

#define Deprecated __attribute__((__deprecated__))

//- breakpoints
#if ARCH_ARM
#define breakpoint() __asm__ volatile("brk #0")
#elif ARCH_x86_64

#if COMPILER_GCC
#define breakpoint() __asm__ volatile("int3")
#endif
#endif

#if COMPILER_MSVC
#include <intrin.h>
#define breakpoint() __debugbreak()
#endif

//-
#define MemCpy(dest, src, len) memcpy((dest), (src), (len))
#define MemSet(dest, len) memset((dest), (1), (len))
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
typedef uint8_t u8;

typedef int8_t s8;
typedef int16_t s16;

typedef int32_t s32;
typedef int64_t s64;

typedef float f32;
typedef double f64;

typedef s32 b32;
typedef s16 b16;
typedef s8 b8;

typedef uintptr_t umm;
typedef intptr_t smm;

//- disable certain msvc errors
#if COMPILER_MSVC
#pragma warning(disable : 6246) // disable the same name within a certain scope
#endif

//-

#define Assert(expression) _assert(expression)

// NOTE: apparantly in macros this strinifys whataever you put after the #
#define AssertLog(expression, ...) do { \
        if (!expression) { \
            Log("%s:%d: %s\n", __FILE__, __LINE__, #expression); \
            Log(__VA_ARGS__);\
            Assert(false);\
        }\
    } \
    while(0)

internal void _assert(bool expression);


internal void _assert(bool expression)
{

    if (!expression) {
        breakpoint();
#if COMPILER_MSVC
        __fastfail(0);
#else
        __builtin_trap(); // NOTE: always stop, breakpoint() is empty on arm without DEBUG
#endif
    }
}






#endif //  BASE_CORE_H
