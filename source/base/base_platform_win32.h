#ifndef BASE_PLATFORM_WIN32_H
#define BASE_PLATFORM_WIN32_H

#define STDIN 0
#define STDOUT 1
#define STDERR 2

//- Shared libraries
internal void *win32_load_library();
internal void  win32_library_get_proc();

//- File I/O
internal b32         win32_file_exists(u8 *path);
internal b32         win32_file_write(String8 data, String8 path);
internal String8     win32_file_load(Arena *arena, String8 file);
internal String8List win32_open_dir(Arena *arena, String8 *original_path); // TODO: ...

internal b32  win32_os_exit();
internal void win32_handle_error(Arena *arena, DWORD error);
internal u64  safe_truncate_64(u64 value);

internal void win32_log(String8List messages);

#define FileLoad(arena, path) win32_file_load(arena, path)
#define FileWrite(data, path) win32_file_write(data, path)
#define FileExists(path) win32_file_exists(path)
#define Win32FileExists(path) win32_file_exists(path)
#define DirLoad(arena, path) win32_open_dir(arena, original_path);

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

#endif
