#ifndef BASE_PLATFORM_LINUX_H
#define BASE_PLATFORM_LINUX_H

//- Types

typedef void(LibSym)(void *);


typedef struct LibrarySymbols LibrarySymbols;
struct LibrarySymbols {
 LibSym *current_sym;
 LibSym *next_sym;
 LibSym *previous_sym;
};

typedef struct SharedLibrary SharedLibrary;
struct SharedLibrary {
 void           *mem;
 cstring         name;
 LibrarySymbols *symbols;
};

typedef struct Directory Directory;
struct Directory {
 String8List *file_names;
};

#define LinuxLoadSharedLibrary(arena, name) linux_load_shared_library(arena, name)
#define LoadSharedLibrary (arena, name) LinuxLoadSharedLibrary(arena, name)
#define PlatformGetCurrentTime(format) linux_get_current_time(format)

//- Shared libraries
internal void *linux_shared_library_get_proc_from_name(SharedLibrary *library, const char *name);
internal void  linux_load_shared_library(Arena *arena);
internal int   linux_shared_library_close(SharedLibrary *library);

internal String8 linux_get_env(const char *var);

//- File I/O
internal b32          linux_file_write(String8 data, String8 path);
internal String8      linux_file_load(Arena *arena, String8 path);
internal b32          linux_file_exists(String8 path);
internal String8List *linux_open_dir(Arena *arena,
                                    String8  *original_path); // TODO: change the return value

internal s64  linux_read(int fd, const void *buf, u64 count);
internal void linux_os_exit() __attribute__((noreturn));

internal s64  linux_log(int fd, const void *buf, u64 count);
internal void linux_log_with_args(const char *format, ...);

#define FileLoad(arena, path) linux_file_load(arena, path)
#define FileWrite(data, path) linux_file_write(data, path)
#define FileExists(path) linux_file_exists(path)
#define DirLoadFileNames(arena, path) linux_open_dir(arena, path)

#if DEBUG
#define OSExit(code) _exit(code)
#define Log(...) linux_log_with_args(__VA_ARGS__)
#define LogStr8(message) Log(Str8Fmt, Str8Arg(message))
#else
#define OSExit(code) Unused(code)
#define Log(...) ((void)0)
#define LogStr8(message) ((void)0)
#endif // disable comments when debug mode is disabled

//- Entrypoint

// variadic so it replaced different params

#define ENTRYPOINT_1(unused) int main(void)
#define ENTRYPOINT_2(argc, argv) int main(int argc, char **args)

#define ENTRYPOINT_PICK(_1, _2, NAME, ...) NAME
#define ENTRYPOINT(...) ENTRYPOINT_PICK(__VA_ARGS__, ENTRYPOINT_2, ENTRYPOINT_1, )(__VA_ARGS__)

#endif
