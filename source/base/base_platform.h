#ifndef BASE_PLATFORM_H
#define BASE_PLATFORM_H

#define MaxPath 260

//- Linux
#if PLATFORM_LINUX || PLATFORM_UNIX


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

 #if 1
 #define LinuxLoadSharedLibrary(arena, name) unix_load_shared_library(arena, name)
 #define LoadSharedLibrary (arena, name) LinuxLoadSharedLibrary(arena, name)

 #define LinuxLoadLink(str, path, size) readlink(str, path, size)

 #define PlatformReadLink(path, size);
 #endif

 //- Shared libraries
 internal void *unix_shared_library_get_proc_from_name(SharedLibrary *library, const char *name);
 internal void  unix_load_shared_library(MemArena *arena);
 internal int   unix_shared_library_close(SharedLibrary *library);
 internal String8 unix_get_env(const char *var);

 //- File I/O
 internal b32          unix_file_write(String8 data, String8 path);
 internal String8      unix_file_load(MemArena *arena, String8 path);
 internal b32          unix_file_exists(String8 path);
 internal String8List *unix_open_dir(MemArena *arena,
 String8  *original_path);
 internal s64  unix_read(int fd, const void *buf, u64 count);
 internal void unix_os_exit() __attribute__((noreturn));

 internal s64  unix_log(int fd, const void *buf, u64 count);
 internal void unix_log_with_args(const char *format, ...);

 #define FileLoad(arena, path) unix_file_load(arena, path)
 #define FileWrite(data, path) unix_file_write(data, path)
 #define FileExists(path) unix_file_exists(path)
 #define DirLoad(arena, path) unix_open_dir(arena, path)

 #if DEBUG
 #define OSExit(code) _exit(code)
 #define Log(...) unix_log_with_args(__VA_ARGS__)
 #define LogStr8(message) Log(Str8Fmt, Str8Arg(message))
 #else
 #define OSExit(code) Unused(code)
 #define Log(...) ((void)0)
 #define LogStr8(message) ((void)0)
 #endif // disable comments when debug mode is disabled

 //- Entrypoint
 #define ENTRYPOINT() int main(int count, char **args)

 //- Windows
 #elif PLATFORM_WINDOWS

 #define STDIN 0
 #define STDOUT 1
 #define STDERR 2

 //- Shared libraries
 internal void *win32_load_library();
 internal void  win32_library_get_proc();

 //- File I/O
 internal b32         win32_file_exists(u8 *path);
 internal b32         win32_file_write(String8 data, String8 path);
 internal String8     win32_file_load(MemArena *arena, String8 file);
 internal String8List win32_open_dir(MemArena *arena, String8 *original_path); // TODO: ...

 internal b32  win32_os_exit();
 internal void win32_handle_error(MemArena *arena, DWORD error);
 internal u64  safe_truncate_64(u64 value);

 internal void win32_log(String8List messages);

 #define FileLoad(arena, path) win32_file_load(arena, path)
 #define FileWrite(data, path) win32_file_write(data, path)
 #define FileExists(path) win32_file_exists(path)
 #define Win32FileExists(path) win32_file_exists(path)
 #define DirLoad(arena, path) win32_open_dir(arena, original_path);

 #define WindowsReadLink(/*optional*/ handle, file_name, size) GetModuleFileNameA(handle, file_name, size)

 // NOTE: passing 0 to GetModuleFileNameA signifies the current executable
 // importatn to check the return value because it can give errors
 #define PlatformReadLink(file_name, size) WindowsReadLink(0, file_name size)

 #define OSExit(code) win32_os_exit()

 #if COMPILER_GCC || COMPILER_CLANG

 #define PRINTF_FORMAT(fmt, args) __attribute__((format(printf, fmt, args)))
 internal void win32_logf(const char *fmt, ...) PRINTF_FORMAT;

 #elif COMPILER_MSVC
 internal void win32_logf(_Printf_format_string_ const char *fmt, ...);

 #define Log(fmt, ...) win32_logf(fmt, ##__VA_ARGS__)

 #endif // COMPILER_GCC || COMPILER_CLANG

 //- Entrypoint
 #if COMPILER_MSVC
 #pragma warning(disable : 28251) // disabled the annotations warning that are defined in winbase.h
 #define ENTRYPOINT(hinstance, prev_instance, cmd_line, show_cmd) \
 int WINAPI WinMain(hinstance, prev_instance, cmd_line, show_cmd)
#endif // COMPIPILER_MSVC DISABLE WARNING

#endif // PLATFORM_LINUX || PLATFORM_WINDOWS

#endif // BASE_PLATFORM_H
